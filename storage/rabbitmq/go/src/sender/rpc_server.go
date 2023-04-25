package main

import(
	"fmt"
	"log"
	"strconv"
	"github.com/streadway/amqp"
)

/*
V6: RPC客户端，向服务端请求队列rpc_queue发送请求。并通过ReplyTo指定response发送的callback queue

(base) M310144TCG8WP:receiver hunk.he$ rabbitmqctl list_bindings
Listing bindings for vhost /...
source_name	source_kind	destination_name	destination_kind	routing_key	arguments
	exchange	rpc_queue	queue	rpc_queue	[]      									 //request queue
	exchange	amq.gen-XuLJdx2CccpEkXONpnf9Rw	queue	amq.gen-XuLJdx2CccpEkXONpnf9Rw	[]   //callback queue

References:
https://www.rabbitmq.com/tutorials/tutorial-six-go.html
*/

func failOnError(err error, msg string) {
	if err != nil {
		log.Fatalf("%s: %s", msg, err)
	}
}

func fib(n int) int {
	if n == 0  {
		return 0
	} else if n == 1 {
		return 1
	} else {
		return fib(n-1) + fib(n-2)
	}
}


func main() {
	fmt.Println("Rpc_server starting...")

	conn, err := amqp.Dial("amqp://guest:guest@localhost:5672/")
	failOnError(err, "Failed to connect to RabbitMQ")
	defer conn.Close()

	ch, err := conn.Channel()
	failOnError(err, "Failed to open a channel")
	defer ch.Close()

	//rpc queue, receive request messages from client
	q, err := ch.QueueDeclare(
		"rpc_queue",  //name
		false,        //durable
		false,        //delete when unused
		false,        //exclusive
		false,        //no-wait
		nil,           //argument
	)
	failOnError(err, "Failed to declare a queue")


	err = ch.Qos(
		1,   //prefetch count
		0,   //prefetch size
		false,  //global
	)
	failOnError(err, "Failed to set Qos")

	msgs, err := ch.Consume(
		q.Name,      //queue
		"",          //consumer 
		false,      //auto-ack
		false,       //exclusive
		false,      //no-local
		false,       //no-wait
		nil,         //args
	)

	failOnError(err, "Failed to register a consumer")

	forever := make(chan bool)

	go func() {
		for d := range msgs {
			n, err := strconv.Atoi(string(d.Body))
			failOnError(err, "Failed to convert body to integer")

			fmt.Printf(" [.] fib(%d)", n)
			response := fib(n)

			err = ch.Publish(
				"",      //exchange
				d.ReplyTo,  //routing key
				false,   //mandatory
				false,    //immediate
				amqp.Publishing{
					ContentType:   "text/plain",
					CorrelationId: d.CorrelationId,
					Body:          []byte(strconv.Itoa(response)),
				})

			failOnError(err, "Failed to publish a message")

			d.Ack(false)
			fmt.Printf("\n [.] response(%d) [.] corrid(%s)", response, d.CorrelationId)
		}
	}()

	fmt.Println(" [*] Awaiting RPC requests")
	<- forever
}
