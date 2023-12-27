package log

import "fmt"

func log(level string, msg string) {
	log_msg := fmt.Sprintf("[%s] %s", level, msg)
	fmt.Println(log_msg)
}

func Debug(msg string) {
	log("DEBUG", msg)
}

func Info(msg string) {
	log("INFO", msg)
}

func Warning(msg string) {
	log("WARN", msg)
}

func Error(msg string) {
	log("ERROR", msg)
}
