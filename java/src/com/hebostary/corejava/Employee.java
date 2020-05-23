package com.hebostary.corejava;

import java.util.Objects;

/**
 * 类注释：This ia employ class.
 * 通用注释：
 * @author hunk
 * @version 1.0
 * ...
*/
public class Employee implements Comparable<Employee>
{
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

    //实现Comparable接口的compareTo方法
    public int compareTo(Employee other)
    {
        return Double.compare(salary, other.salary);
    }

    public double getSalary() {
        return salary;
    }

    public void raiseSalary(double byPercent)
    {
        double raise = salary * byPercent / 100;
        salary += raise;
    }

    public String getName() {
        return name;
    }

    // 1.显示参数命名为otherObject
    // 显示参数类型不能声明成Employee，否则无法覆盖Object类的equals方法，而是定义了一个完全无关的方法
    // 为了避免发生类型错误，可以使用@Override对覆盖超类的方法进行标记
    @Override 
    public boolean equals(Object otherObject)
	{
        // 2.检测this与otherObject是否引用同一个对象，实际上这是一个优化
        if (this == otherObject) return true;
        
        // 3.检测otherObject是否为null
        if (otherObject == null) return false;
        
        // 4.1 比较this与otherObject是否属于同一个类。如果equals的语义在每个子类中都有改变，
        // 就使用getClass检测，也就是上面的相等语义1的处理.
        if (getClass() != otherObject.getClass()) return false;
        
        // or 4.2 如果所有子类都有统一的相等语义，就使用instanceof检测
        // 也就是上面的相等语义2的处理：
        // if(!(otherObject instanceof Employee)) return false;
        
        // 5.将otherObject转换为相应的类类型变量：
        Employee other = (Employee) otherObject;
        
        // 6.如果在子类中重新定义equals，就要在其中包含调用super.equals(other)。
        // 并对所有需要比较的域进行比较，基本类型域使用==比较，对象域使用equals比较：
        return Objects.equals(name, other.name)
        	&& salary == other.salary;
    }

    public int hashCode()
    {
        return Objects.hash(name, salary);
    }

    public String toString()
    {
        return getClass().getName() + "[name=" + name + ",id=" + id +
        ", salary=" + salary + "]";
    }

    /**
     * Execute test case of this class
     * @param None
     * @return void
    */
    public static void Test() {
        Common.PrintClassHeader(Employee.class.getName());
        Employee employee1 = new Employee("hunk", 6000);
        Employee employee2 = new Employee("jack", 6000);
        //com.hebostary.corejava.Employee[name=hunk,id=0, salary=6000.0]
        System.out.println(employee1); 
        //com.hebostary.corejava.Employee[name=jack,id=1, salary=6000.0]
        System.out.println(employee2); //1

        System.out.println(employee1.equals(employee1));//true
        System.out.println(employee1.equals(employee2));//false
    }
}