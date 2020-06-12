package com.hebostary.corejava.concurrence;

import com.hebostary.corejava.*;

public class ThreadInterrupt {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());

        StartNewThread();
    }

    public static void StartNewThread() {
        Runnable r = () -> {
            try {
                Common.printThreadInfo(Thread.currentThread(), "Hi...");
                Thread.sleep(1000);
                Common.printThreadInfo(Thread.currentThread(), "Sleep done...");;//不会被执行
            } catch (InterruptedException e) {
                //如果在每次工作迭代之后都调用 sleep 方法（或者其他的可中断方法，) islnterrupted 检测
                //既没有必要也没有用处。 如果在中断状态被置位时调用 sleep 方法，它不会休眠。 
                //相反，它将清除这一状态并拋出 IntemiptedException。
                Common.printThreadInfo(Thread.currentThread(), "interrupted");
            } finally {
                //clean up
                Common.printThreadInfo(Thread.currentThread(), "cleaning up...");
            }
        };

        Thread t = new Thread(r, "child thread 1");
        Common.printThreadInfo(t, "before starts...");//[INFO] I'm thread[ child thread 1], id[ 12 ], state[ NEW ];before starts...
        t.start();
        t.interrupt();

        Common.printThreadInfo(Thread.currentThread(), "Hi...");
    }
}