package main

import(
	"fmt"
	"log"
	"time"
	"bytes"
	"math/rand"
	"github.com/streadway/amqp"
)

/*
V5: 使用exchange（topic）通过routing key（binding key）把消息路由到路由到不同的queue，并且实现多重条件的匹配

References:
https://www.rabbitmq.com/tutorials/tutorial-five-go.html
*/


func failOnError(err error, msg string) {
	if err != nil {
		log.Fatalf("%s: %s", msg, err)
	}
}

var routingKeys = []string{
	"error.#", 
	"error.warning.info",
	"error.warning",
	"error",
}

func main() {
	fmt.Println("Receiver starting...")

	conn, err := amqp.Dial("amqp://guest:guest@localhost:5672/")
	failOnError(err, "Failed to connect to RabbitMQ")
	defer conn.Close()

	ch, err := conn.Channel()
	failOnError(err, "Failed to open a channel")
	defer ch.Close()

	err = ch.ExchangeDeclare(
		"logs_topic",   // name
		"topic",  // type
		true,     // durable
		false,    //auto-deleted
		false,    //internal
		false,    //no-wait
		nil,      //arguments
	)
	failOnError(err, "Failed to declare a exchange")

	q, err := ch.QueueDeclare(
		"", // random name
		false,   // durable, once we disconnect the consumer the queue should be automatically deleted.
				//这里不对queue做持久化，publisher同时发送大量message时，message很可能还没有被分发到subscriber，queue就被删除了
		false,   // delete when unused
		true,   // exclusive, When the connection that declared it closes, the queue will be deleted because it is declared as exclusive.
		false,   // no-wait
		nil,     // arguments
	)
	failOnError(err, "Failed to declare a queue")

	//将exchange和queue绑定在一起
	rd := rand.New(rand.NewSource(time.Now().UnixNano()))
	routingKey := routingKeys[ rd.Intn(100) % 4]
	fmt.Println("Binding key:", routingKey)
	err = ch.QueueBind(
		q.Name,  //queue name
		routingKey,      //routing key
		"logs_topic",   // exchange
		false,
		nil,  
	)
	failOnError(err, "Failed to bind a exchange")

	msgs, err := ch.Consume(
		q.Name, // queue
		"",     // consumer
		false,   // auto-ack 如果设置成自动应答，而message的处理时间比较长，后到的message可能会被丢失
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
