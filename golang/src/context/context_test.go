package context

import (
	"context"
	"fmt"
	"testing"
	"time"
)

func context_timeout(parentTimeout, childTimeout time.Duration) {
	ctx := context.Background()
	before := time.Now()
	parentCtx, _ := context.WithTimeout(ctx, parentTimeout*time.Millisecond)
	go func() {
		childCtx, _ := context.WithTimeout(parentCtx, childTimeout*time.Millisecond)
		select {
		case <-childCtx.Done():
			after := time.Now()
			fmt.Println("childCtx.Done(): ", after.Sub(before).Milliseconds())
		}
	}()

	select {
	case <-parentCtx.Done():
		after := time.Now()
		fmt.Println("parentCtx.Done(): ", after.Sub(before).Milliseconds())
	}
}

func TestContextCase1(t *testing.T) {
	context_timeout(100, 300)
}

func TestContextCase2(t *testing.T) {
	context_timeout(500, 300)
}
