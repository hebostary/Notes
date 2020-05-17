package com.hebostary.corejava;

import java.util.Arrays;
import java.util.Scanner;

public class EnumTest {
    public static void Test() {
       Common.PrintHeader(EnumTest.class.getName());
       Scanner in = new Scanner(System.in);
       System.out.println("Enter a size: (SMALL, MEDIUM, LARGE)");
       String input = in.next().toUpperCase();//input: small

       //静态方法Enum.valueOf是toString方法的逆方法
       //将size设置成Size.SMALL
       Size size = Enum.valueOf(Size.class, input);

       //自动调用size.toString()，返回枚举常量名
       System.out.println("size=" + size);//size=SMALL
       System.out.println("abbreviation=" + size.getAbbreviation());//S
       
       //每个枚举类型都有一个静态的values方法，返回一个包含全部枚举值的数组
       Size[] values = Size.values();
       System.out.println(Arrays.toString(values));//[SMALL, MEDIUM, LARGE]

       //ordinal方法返回enum声明中枚举常量的位置，位置从0开始计数
       System.out.println(size.ordinal());//0

       in.close();
    }
}

enum Size
{
    //括号里可以存放实例的缩写
    SMALL("S"), MEDIUM("M"), LARGE("L");

    private Size(String abbreviation) {
        this.abbreviation = abbreviation;
    }
    public String getAbbreviation() {
        return abbreviation;
    }

    private String abbreviation;
}