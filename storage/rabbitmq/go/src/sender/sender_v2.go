package main

import(
	"fmt"
	"log"

	"github.com/streadway/amqp"
)

/*
V2: 对消息做持久化

References:
https://www.rabbitmq.com/tutorials/tutorial-two-go.html
*/

func failOnError(err error, msg string) {
	if err != nil {
		log.Fatalf("%s: %s", msg, err)
	}
}

func main() {
	fmt.Println("Sender starting...")

	conn, err := amqp.Dial("amqp://guest:guest@localhost:5672/")
	failOnError(err, "Failed to connect to RabbitMQ")
	defer conn.Close()

	ch, err := conn.Channel()
	failOnError(err, "Failed to open a channel")
	defer ch.Close()

	q, err := ch.QueueDeclare(
		"task_queue", // name
		true,   // durable,对消息持久化，即使MQ挂掉重启后消息也不会丢失
		false,   // delete when unused
		false,   // exclusive
		false,   // no-wait
		nil,     // arguments
	)
	failOnError(err, "Failed to declare a queue")

	body := "Message"
	for index := 0; index < 10; index++ {
		body += "."
		err = ch.Publish(
			"",     // exchange
			q.Name, // routing key, 如果没有指定exchange,消息就会被路由到routeing key指定的queue
			false,  // mandatory
			false,  // immediate
			amqp.Publishing{
				DeliveryMode: amqp.Persistent,
				ContentType: "text/plain",
				Body:        []byte(body),
			})
		log.Printf(" [x] Sent %s", body)
		failOnError(err, "Failed to publish a message")
	}

}
