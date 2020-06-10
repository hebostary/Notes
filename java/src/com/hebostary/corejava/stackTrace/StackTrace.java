package com.hebostary.corejava.stackTrace;

import com.hebostary.corejava.*;

/**
 * 分析堆栈轨迹
*/
public class StackTrace {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        factorial(3);
    }

    public static int factorial(int n) {
        System.out.println("factorial(" + n + ")");
        Throwable t = new Throwable();
        StackTraceElement[] fTraceElements = t.getStackTrace();
        for (StackTraceElement stackTraceElement : fTraceElements) {
            System.out.println(stackTraceElement);
        }
        int r;
        if (n <= 1) r = 1;
        else r = n * factorial(n - 1);
        System.out.println("return " + r);
        return r;
    }
}