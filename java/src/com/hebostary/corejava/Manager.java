package com.hebostary.corejava;

public class Manager extends Employee {
    //子类继承父类的方法和域，也可以添加自己的方法和域
    private double bonus;

    //默认构造器
    public Manager() {
        super("NoneName", 100);
        bonus = 0;
    }

    public Manager(String n, double s, double b) 
    {
        //通过super调用父类中的构造器方法
        //使用super调用构造器的语句必须是子类构造器的第一条语句
        super(n, s);
        bonus = b;
    }

    public void setBonus(double b) {
        bonus = b;
    }

    //覆盖父类中的方法实现
    public double getSalary()
    {
        //子类中无法直接访问父类中的private实例域
        //通过super来调用父类中的public方法
        double baseSalary = super.getSalary();
        return baseSalary + bonus;
    }

    public static void Test() {
        Common.PrintHeader(Manager.class.getName());
        Employee[] staff = new Employee[3];
        staff[0] = new Manager("Hunk", 6000, 3000);
        staff[1] = new Employee("Jack", 7000);
        staff[2] = new Employee("Harry", 8000);
        for (Employee e : staff) {
            System.out.printf("%s's total salary: %8.2f.\n", e.getName(), e.getSalary());
        }
    }
}