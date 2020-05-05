package com.hebostary.corejava;

/**
 * 类注释：This ia employ class.
 * 通用注释：
 * @author hunk
 * @version 1.0
 * ...
*/
public class Employee {
    //1.在声明中赋值初始化
    private static int nextId = 0;
    private int id;
    private String name;
    private double salary;

    //2.初始化块（initialization block）
    //在一个类中可以包含多个代码块，只要构造类的对象，这些块就会被执行。
    //首先运行初始化块，然后才运行构造器的主体部分，观察id的变化。
    {
        id = nextId;
    }

    //3.在构造器中设置初始值
    public Employee(String n, double s)
    {
        name = n;
        salary = s;
        nextId++;
    }

    /**
     * Execute test case of this class
     * @param None
     * @return void
    */
    public static void Test() {
        Common.PrintHeader("Employee");
        Employee employee1 = new Employee("hunk", 6000);
        Employee employee2 = new Employee("jack", 6000);
        System.out.println(employee1.id); //0
        System.out.println(employee2.id); //1
    }
}