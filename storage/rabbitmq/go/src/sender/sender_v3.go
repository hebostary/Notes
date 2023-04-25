package main

import(
	"fmt"
	"log"

	"github.com/streadway/amqp"
)

/*
V3: 使用exchange（fanout）把一个消息路由到多个queue（消息广播）

References:
https://www.rabbitmq.com/tutorials/tutorial-three-go.html
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

	//如果exchange不存在则自动创建
	err = ch.ExchangeDeclare(
		"logs",   // name
		"fanout",  // type
		true,     // durable
		false,    //auto-deleted
		false,    //internal
		false,    //no-wait
		nil,      //arguments
	)
	failOnError(err, "Failed to declare a exchange")

	body := "Message"
	for index := 0; index < 10; index++ {
		body += "."
		err = ch.Publish(
			"logs",     // exchange
			"", // routing key, 如果没有指定exchange,消息就会被路由到routeing key指定的queue
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
