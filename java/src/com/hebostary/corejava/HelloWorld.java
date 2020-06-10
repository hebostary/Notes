package com.hebostary.corejava;

import java.io.PrintWriter;
import java.math.BigInteger;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.Date;
import java.util.Scanner;

/**
 * HelloWorld
 */
public class HelloWorld {
    public static final String HELLO_STRING = "Hello Java!!!";
    
    public static void Test() {
        Common.PrintClassHeader(HelloWorld.class.getName());

        System.out.println(HELLO_STRING);

        System.out.println("--------------1---------------");
        int age = 13;
        String user = "hunk" + age;
        System.out.println(user);

        System.out.println("hunk13".equals(user)); //true
        System.out.println("HUNK13".equalsIgnoreCase(user)); //true，比较时忽略大小写

        System.out.println(user == "hunk13"); //false

        String user1 = "hunk13";
        String user2 = "hunk13";
        System.out.println(user == user1); //false
        System.out.println(user1 == user2); //true

        System.out.println("-------------2----------------");
        System.out.println(user2.length());//6
        System.out.println(user2.codePointCount(0, user2.length()));//6

        System.out.println(user2.charAt(0));//h

        System.out.println("-------------3----------------");
        String str = "tringbuilder";
        StringBuilder builder = new StringBuilder();
        builder.append('S');
        builder.append(str);
        String compString = builder.toString();
        System.out.println(compString); //Stringbuilder

        System.out.println("-------------4----------------");
        String message = String.format("Hello %s %d", "java", 11);
        System.out.println(message); //Hello java 11
        System.out.printf("%tc\n", new Date()); //周日 5月 03 10:17:45 CST 2020
        System.out.printf("%tF\n", new Date()); //2020-05-03
        System.out.printf("%tD\n", new Date()); //05/03/20
        System.out.printf("%tT\n", new Date()); //10:17:46

        System.out.println("--------------5---------------");
        try {
            String fileString = "myFile.txt";
            //写文件
            PrintWriter out = new PrintWriter(fileString, "UTF-8");
            out.println(message);
            out.println("Good morning!");
            out.close();

            //读文件
            Scanner in = new Scanner(Paths.get(fileString), "UTF-8"); //Paths.get()根据给定的路径名构造一个Path
            System.out.println(in.nextLine()); //Hello java 11，读取一行
            System.out.println(in.next()); //Good，只读取一个单词，以空白符作为分隔符
            in.close();
        } catch (Exception e) {
            //TODO: handle exception
        }

        System.out.println("-------------6----------------");
        BigInteger a = BigInteger.valueOf(100);
        BigInteger b = BigInteger.valueOf(200);
        BigInteger c = a.add(b); //a + b
        BigInteger d = c.multiply(a.add(BigInteger.valueOf(2))); //d = c * (a + 2)
        System.out.println(c); //300
        System.out.println(d); //30600

        System.out.println("-------------7----------------");
        //声明一个数组变量，但是并未将其初始化为一个真正的数组，在调用new初始化之前不能使用
        int[] a0;
        //使用new运算符创建数组：
        //数值元素都被初始化为0，布尔元素都被初始化为false；
        //对象数组则初始化为null，表示这些元素还未存放任何对象
        int[] a1 = new int[5];
        String[] a2 = new String[5];
        System.out.println(Arrays.toString(a1)); //[0, 0, 0, 0, 0]
        System.out.println(Arrays.toString(a2)); //[null, null, null, null, null]
        //对数组元素逐个进行初始化
        for (int i = 0; i < a1.length; i++) {
            a1[i] = i;
        }

        //创建数组对象并同时赋予初始值的简写形式，这种概况不需要使用new
        int[] arr1 = {1, 2, 3, 4};

        //在不创建新变量的情况下重新初始化一个数组
        //new int[] { ... }实际上是创建了一个匿名数组并完成初始化
        //这条语句实际上等价于下面两条：
        //int[] anonymous = {10, 20, 30, 40, 50};
        //arr1 = anonymous;
        arr1 = new int[] {10, 20, 30, 40, 50};

        //数组浅拷贝：java允许将一个数组变量拷贝给另一个数组变量。
        //这时，两个变量将引用同一个底层数组
        int[] arr2 = arr1;
        arr2[3] = 400;
        System.out.println(Arrays.toString(arr1)); //[10, 20, 30, 400, 50]

        //数组深拷贝：如果希望将数组的所有值拷贝到一个新的数组中，
        //就使用Arrays类的copyOf方法：
        //第二个参数是数组的长度，这个方法通常用于增加数组大小
        //如果数组元素是数值型，多余元素被赋值为0；布尔型赋值为false。
        //如果长度小于原始数组长度，则只拷贝最前面的数据元素。
        int[] arr3 = Arrays.copyOf(arr1, arr1.length);
        arr3[3] = 4000;
        System.out.println(Arrays.toString(arr1)); //[10, 20, 30, 400, 50]

        //数组排序：这个方法采用了优化的快排算法
        Arrays.sort(arr1);
        System.out.println(Arrays.toString(arr1)); //[10, 20, 30, 50, 400]

        //声明二维数组，在调用new初始化之前不能使用
        double[][] balances;
        //new初始化
        balances = new double[3][3];
        //知道数组元素的初始化
        int[][] magicSquare = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
        };

        //for each循环蹦年自动处理二维数组中的每一个元素，需要两个嵌套的循环
        for (int[] row : magicSquare) {
            for (int ele : row) {
                System.out.printf("%d ", ele);
            }
        }
        System.out.println(Arrays.deepToString(magicSquare));//[[1, 2, 3], [4, 5, 6], [7, 8, 9]]


        //可以交换数组的不同行
        int[] temp = magicSquare[0];
        magicSquare[0] = magicSquare[1];
        magicSquare[1] = temp;
        System.out.println(Arrays.deepToString(magicSquare));//[[4, 5, 6], [1, 2, 3], [7, 8, 9]]

        //不规则数组
        int MAX = 5;
        //先分配一个具有所含行数的数组
        int[][] odds = new int[MAX + 1][];
        //依次分配各个行
        for (int i = 0; i <= MAX; i++) {
            odds[i] = new int[i + 1];
        }
        System.out.println(Arrays.deepToString(odds));
        //[[0], [0, 0], [0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0]]
    }
}