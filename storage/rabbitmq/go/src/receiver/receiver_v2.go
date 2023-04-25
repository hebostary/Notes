package main

import(
	"fmt"
	"log"
	"time"
	"bytes"
	"github.com/streadway/amqp"
)

/*
V2: 对消息做持久化,并且取消对消息的自动应答

References:
https://www.rabbitmq.com/tutorials/tutorial-two-go.html
*/

func failOnError(err error, msg string) {
	if err != nil {
		log.Fatalf("%s: %s", msg, err)
	}
}

func main() {
	fmt.Println("Receiver starting...")

	conn, err := amqp.Dial("amqp://guest:guest@localhost:5672/")
	failOnError(err, "Failed to connect to RabbitMQ")
	defer conn.Close()

	ch, err := conn.Channel()
	failOnError(err, "Failed to open a channel")
	defer ch.Close()

	//https://www.rabbitmq.com/queues.html#temporary-queues
	//Declaring a queue will cause it to be created if it does not already exist
	//The declaration will have no effect if the queue does already exist and its 
	//attributes are the same as those in the declaration. When the existing queue 
	//attributes are not the same as those in the declaration a channel-level exception with code 406 
	q, err := ch.QueueDeclare(
		"task_queue", // name, Queue names starting with "amq." are reserved for internal use by the broker. 
		//Attempts to declare a queue with a name that violates this rule will result in a channel-level exception with reply code 403
		true,   // durable, 对消息持久化，即使MQ挂掉重启后消息也不会丢失
		false,   // delete when unused
		false,   // exclusive, used by only one connection and the queue will be deleted when 
				//that connection closes due to underlying TCP connection loss
				//不同的连接想要使用独占的队列时会导致channel-level异常：RESOURCE_LOCKED
		false,   // no-wait
		nil,     // arguments
	)
	failOnError(err, "Failed to declare a queue")

	//对worker处理消息限流
	//如果不做限流，MQ仅仅是不停地向worker轮询分发新的消息，而不去看worker还有多少未应答（没处理完）的消息
	//如果消息的工作负载不同，很可能会导致部分worker很闲，然而部分worker很忙
	//比如两个worker，奇数的消息的工作负载都很重，偶数的工作负载都很轻，就会导致上面的情况
	err = ch.Qos(
		1, 	 	//prefetch count，同时只处理一个消息,在worker处理完并应答之前，MQ不会dispatch新的消息
		0,  	//prefetch size
		false, 	//global
	)
	failOnError(err, "Failed to set Qos")

	msgs, err := ch.Consume(
		q.Name, // queue
		"",     // consumer
		false,   // auto-ack
		false,  // exclusive
		false,  // no-local
		false,  // no-wait
		nil,    // args
	)
	failOnError(err, "Failed to register a consumer")

	forever := make(chan bool)

	go func() {
		for d := range msgs {
			log.Printf("Received a message: %s", d.Body)
			dot_count := bytes.Count(d.Body, []byte("."))
			t := time.Duration(dot_count)
			time.Sleep(t * time.Second)
			log.Printf("Done")
			d.Ack(false)  //处理完成后应答消息
		}
	}()

	log.Printf(" [*] Waiting for messages. To exit press CTRL+C")
	<-forever
}
