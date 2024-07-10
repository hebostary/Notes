package smtp

import (
	"crypto/tls"
	"fmt"

	"gopkg.in/gomail.v2"
)

//var server = "smtp.qq.com"

var server = "183.47.101.192"
var port = 587
var sender = "986715243@qq.com"
var pass = "okxnxuyziedrbfff"

func run_gosmtp_client() {

	m := gomail.NewMessage()

	//发送人
	m.SetHeader("From", sender)
	//接收人
	m.SetHeader("To", sender)
	//抄送人
	//m.SetAddressHeader("Cc", "xxx@qq.com", "xiaozhujiao")
	//主题
	m.SetHeader("Subject", "小佩奇")
	//内容
	m.SetBody("text/html", "<h1>新年快乐</h1>")
	//附件
	//m.Attach("./myIpPic.png")

	//拿到token，并进行连接,第4个参数是填授权码
	d := gomail.NewDialer(server, port, sender, pass)

	d.TLSConfig = &tls.Config{InsecureSkipVerify: true}

	// 发送邮件
	if err := d.DialAndSend(m); err != nil {
		fmt.Printf("DialAndSend err %v:", err)
		panic(err)
	}
	fmt.Printf("send mail success\n")
}
