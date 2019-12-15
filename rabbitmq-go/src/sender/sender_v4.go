package main

import(
	"fmt"
	"log"

	"github.com/streadway/amqp"
)

/*
V4: 使用exchange（direct）通过routing key（binding key）把消息路由到路由到不同的queue

References:
https://www.rabbitmq.com/tutorials/tutorial-four-go.html
*/

func failOnError(err error, msg string) {
	if err != nil {
		log.Fatalf("%s: %s", msg, err)
	}
}

var routingKeys = []string{"odd", "even"}

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
		"logs_direct",   // name
		"direct",  // type
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
		routingKey := routingKeys[len(body)%2]
		err = ch.Publish(
			"logs_direct",     // exchange
			routingKey, // routing key
			false,  // mandatory
			false,  // immediate
			amqp.Publishing{
				DeliveryMode: amqp.Persistent,
				ContentType: "text/plain",
				Body:        []byte(body),
			})
		log.Printf(" [x] Sent %s, Routing key :%s", body, routingKey)
		failOnError(err, "Failed to publish a message")
	}

}
