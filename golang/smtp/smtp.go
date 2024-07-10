package smtp

import (
	"crypto/tls"
	"errors"
	"fmt"
	"net/smtp"
	"strings"
)

func run_smtp_client() {
	//smtp_server := "smtp.qq.com"
	smtp_server := "183.47.101.192"
	smtp_port := "25" //"587"

	senders_email := "986715243@qq.com"
	senders_password := "okxnxuyziedrbfff"

	recipient_email := "986715243@qq.com"
	message := []byte("To: " + recipient_email + "\r\n" +
		"From: " + senders_email + "\r\n" +
		"Subject: Go SMTP Test\r\n" +
		"\r\n" +
		"Hello,\r\n" +
		"测试邮件")
	auth := smtp.PlainAuth("", senders_email, senders_password, smtp_server)

	//err := smtp.SendMail(smtp_server+":"+smtp_port, auth, senders_email, []string{recipient_email}, message)
	err := sendMail(smtp_server+":"+smtp_port, auth, senders_email, []string{recipient_email}, message)
	if err != nil {
		panic(err)
	}

	fmt.Println("Email sent successfully!")

}

func validateLine(line string) error {
	if strings.ContainsAny(line, "\n\r") {
		return errors.New("smtp: A line must not contain CR or LF")
	}
	return nil
}

func sendMail(addr string, a smtp.Auth, from string, to []string, msg []byte) error {
	if err := validateLine(from); err != nil {
		return err
	}
	for _, recp := range to {
		if err := validateLine(recp); err != nil {
			return err
		}
	}
	c, err := smtp.Dial(addr)
	if err != nil {
		return err
	}
	defer c.Close()

	err = c.Hello("localhost")
	if err != nil {
		return err
	}

	if ok, _ := c.Extension("STARTTLS"); ok {
		config := &tls.Config{InsecureSkipVerify: true}
		if err = c.StartTLS(config); err != nil {
			return err
		}
	}
	if a != nil {
		if err = c.Auth(a); err != nil {
			return err
		}
	}
	if err = c.Mail(from); err != nil {
		return err
	}
	for _, addr := range to {
		if err = c.Rcpt(addr); err != nil {
			return err
		}
	}
	w, err := c.Data()
	if err != nil {
		return err
	}
	_, err = w.Write(msg)
	if err != nil {
		return err
	}
	err = w.Close()
	if err != nil {
		return err
	}
	return c.Quit()
}
