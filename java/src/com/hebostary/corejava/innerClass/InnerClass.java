package com.hebostary.corejava.innerClass;

import java.util.Date;

import javax.swing.JOptionPane;
import javax.swing.Timer;

import java.awt.event.*;

import com.hebostary.corejava.*;

public class InnerClass {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        TestTalkingClock();
    }

    public static void TestTalkingClock() {
        TalkingClock clock = new TalkingClock(1000, true);
        clock.start();

        //弹窗保持程序运行
        JOptionPane.showMessageDialog(null, "Quit program?");
        System.exit(0);
    }
}

/**
 * TalkingClock
 */
class TalkingClock
{

    private int interval;
    private boolean beep;

    /**
     * 构造一个talking clock
    */
    public TalkingClock(int interval, boolean beep) {
        this.interval = interval;
        this.beep = beep;
    }

    public void start() {
        ActionListener listener = new TimerPrinter();
        Timer t = new Timer(interval, listener);
        t.start();

        /*
            局部内部类
        */
        //局部内部类不能用public或private访问修饰符进行说明
        //它的作用域被限定在这个局部类的块中，
        //比如这里只能在start函数里使用LocalTimerPrinter
        //与其他内部类相比较，局部类不仅能访问包含它们的外部类，还可以访问局部变量
        //不过，这些局部变量必须事实上为final
        class LocalTimerPrinter implements ActionListener
        {
            //内部类中声明的所有静态域都必须是final
            private static final String message = "LocalTimerPrinter";
    
            public void actionPerformed (ActionEvent event) {
                System.out.println("At the tone, the time is " + new Date());
                //注意，这里的beep是start方法的参数变量，而start方法会提前退出，
                //beep参数变量也随之消失，为了保证局部内部类的访问
                //编译器必须检测对局部变量的访问，为每一个变量建立相应的数据域
                //并将局部变量拷贝到构造器中，以便将这些数据域初始化为局部变量的副本
                if (beep) {
                    System.out.println("Beeping 3... " + message);
                }
            }
        }

        ActionListener listener1 = new LocalTimerPrinter();
        Timer t1 = new Timer(interval, listener1);
        t1.start();

        /*
            匿名内部类
        */
        //写法的含义：创建一个实现 ActionListener 接口的类的新
        //对象，需要实现的方法 actionPerformed 定义在括号内。
        //由于构造器的名字必须与类名相同，而匿名类没有类名，所以匿名类不能有构造器。
        //相比之下，其实Lambda表达式更有吸引力
        ActionListener listener2 = new ActionListener() {
            private static final String message1 = "AnonymousTimerPrinter";
            public void actionPerformed (ActionEvent event) {
                System.out.println("At the tone, the time is " + new Date());
                if (beep) {
                    System.out.println("Beeping 4... " + message1);
                }
            }
        }; //注意这里的分号

        Timer t2 = new Timer(interval, listener2);
        t2.start();
    }

    /*
        普通内部类
    */
    //这个内部类实际上会被编译器翻译成TalkingClock$TimePrinter.class
    public class TimerPrinter implements ActionListener
    {
        //内部类中声明的所有静态域都必须是final
        private static final String message = "TimerPrinter";

        public void actionPerformed (ActionEvent event) {
            System.out.println("At the tone, the time is " + new Date());
            //在内部类中直接访问外围类的数据
            //注意，这里访问是TalkingClock对象的私有域beep
            if (beep) {
                System.out.println("Beeping 1... " + message);
            }
            //也可以这样写
            //if (TalkingClock.this.beep) {
            //    System.out.println("Beeping 2... " + message);
            //}
        }

        //内部类不能有static方法
    }
}