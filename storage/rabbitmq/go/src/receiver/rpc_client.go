package main

import(
	"fmt"
	"log"
	"time"
	"os"
	"strconv"
	"math/rand"
	"strings"
	"github.com/streadway/amqp"
)

/*
V6: RPC客户端，向服务端请求队列rpc_queue发送请求。并通过ReplyTo指定response发送的callback queue

References:
https://www.rabbitmq.com/tutorials/tutorial-six-go.html
*/


func failOnError(err error, msg string) {
	if err != nil {
		log.Fatalf("%s: %s", msg, err)
	}
}

func randomString(l int) string {
	bytes := make([]byte, l)
	for i := 0; i < l; i++ {
		bytes[i] = byte(randInt(65, 90))
	}

	return string(bytes)
}

func randInt(min int, max int) int {
	return min + rand.Intn(max-min)
}

func fibonacciRPC(n int) (res int, err error) {
	conn, err := amqp.Dial("amqp://guest:guest@localhost:5672/")
	failOnError(err, "Failed to connect to RabbitMQ")
	defer conn.Close()

	ch, err := conn.Channel()
	failOnError(err, "Failed to open a channel")
	defer ch.Close()

	//rpc callback queue, for receiving response from rpc server
	//every client with a callback queue
	q, err := ch.QueueDeclare(
		"",  //name
		false,        //durable
		false,        //delete when unused
		true,        //exclusive
		false,        //no-wait
		nil,           //argument
	)
	failOnError(err, "Failed to declare a queue")

	msgs, err := ch.Consume(
		q.Name,      //queue
		"",          //consumer 
		true,      //auto-ack
		false,       //exclusive
		false,      //no-local
		false,       //no-wait
		nil,         //args
	)
	failOnError(err, "Failed to register a consumer")

	corrId := randomString(32)

	err = ch.Publish(
		"",      //exchange
		"rpc_queue",  //routing key 
		false,   //mandatory
		false,    //immediate
		amqp.Publishing{
			ContentType:   "text/plain",
			CorrelationId: corrId,   //every request and one response has a correlation id
			ReplyTo:       q.Name,   //tell rpc server write the response to callback queue for this client
			Body:          []byte(strconv.Itoa(n)),
		})

	failOnError(err, "Failed to publish a message")
	fmt.Printf("\n [.] Published an request, corrid(%s), response queue name(%s)", corrId, q.Name)

	for d := range msgs {
		if corrId == d.CorrelationId {
			res, err = strconv.Atoi(string(d.Body))
			failOnError(err, "Failed to convert body to integer")
			fmt.Printf("\n [.] Received response corrid(%s), body(%d)", corrId, res)
			break
		}
	}

	return
}

func main() {
	fmt.Println("Rpc_client starting...")

	rand.Seed(time.Now().UTC().UnixNano())

	n := bodyFrom(os.Args)

	fmt.Printf("\n [x] Requesting fib(%d)", n)
	res, err := fibonacciRPC(n)
	failOnError(err, "Failed to handle RPC request.")
	
	fmt.Printf("\n [.] Got %d\n", res)
}

func bodyFrom(args []string) int {
	var s string
	if (len(args) < 2) || os.Args[1] == "" {
		s = "30"
	} else {
		s = strings.Join(args[1:], "")
	}

	n, err := strconv.Atoi(s)
	failOnError(err, "Failed to convert arg to integer.")
	return n
}
