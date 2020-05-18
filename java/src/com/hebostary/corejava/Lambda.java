package com.hebostary.corejava;

import java.util.Arrays;
import java.util.Date;

import javax.swing.JOptionPane;
import javax.swing.Timer;

public class Lambda {
    private static void sortLambda() {
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
        Timer t = new Timer(1000, event ->
            System.out.println("Time time is " + new Date()));

        t.start();

        JOptionPane.showMessageDialog(null, "Quit program?");
    }
    public static void Test() {
        Common.PrintHeader(Lambda.class.getName());
        sortLambda();
        timerLambda();
        System.exit(0);
    }
}