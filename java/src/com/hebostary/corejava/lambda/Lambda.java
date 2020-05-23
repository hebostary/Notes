package com.hebostary.corejava.lambda;

import java.util.Arrays;
import java.util.Date;

import javax.swing.JOptionPane;
import javax.swing.Timer;

import com.hebostary.corejava.*;

public class Lambda {
    private static void sortLambda() {
        Common.PrintMethodHeader("sortLambda");
        String[] planets = new String[] {
            "Mercury", "Venus", "Earth", "Mars"
        };
        System.out.println(Arrays.toString(planets));
        System.out.println("Sorted in dictionary order:");
        Arrays.sort(planets);
        System.out.println(Arrays.toString(planets));

        System.out.println("Sorted by length:");
        Arrays.sort(planets, (first, second) -> first.length() - second.length());
        System.out.println(Arrays.toString(planets));
    }

    private static void timerLambda() {
        Common.PrintMethodHeader("timerLambda");
        Timer t = new Timer(1000, event ->
            System.out.println("Time time is " + new Date()));
        t.start();

        JOptionPane.showMessageDialog(null, "Quit program?");
    }

    //重复执行某个动作
    private static void repeat(int n, Runnable action) {
        for (int i = 0; i < n; i++) {
            action.run();
        }
    }

    private static void runnableLambda() {
        Common.PrintMethodHeader("runnableLambda");
        repeat(5, () -> System.out.println("Hello Runnable Lambda."));
    }
    public static void Test() {
        //Common.PrintClassHeader(Lambda.class.getName());
        //用匿名对象在静态方法中获取类名
        //new Object(){}建立Object的一个匿名子类的一个匿名对象
        //getEnclosingClass则得到其外围类，也就是包含这个静态方法的类
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        sortLambda();
        runnableLambda();
        timerLambda();
        System.exit(0);
    }
}