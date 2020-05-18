package com.hebostary.corejava;

import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Arrays;
import java.util.Scanner;

public class Reflection {
    private static void testClass() {
        Employee employee = new Employee("Grace", 10000);
        Manager manager= new Manager("Hunk", 10000, 5000);

        //1.从具体的类对象获得相应的Class对象 
        //Object类中的getClass()方法将会返回一个Class类型的实例
        //如同Employee/Manager对象表示一个特定的雇员属性一样，
        //一个Class对象将表示一个特定类的属性**
        Class class1 = employee.getClass();
        Class class2 = manager.getClass();

        try {
            ////获取类中指定名字的域（不能获取超类的成员）
            Field field = class1.getDeclaredField("name");

            //反射机制的默认行为受限于 Java 的访问控制。
            //因此需要调用setAccessible方法覆盖访问控制
            //setAccessible 方法是 AccessibleObject 类中的一个方法， 
            //它是 Field、 Method 和 Constructor类的公共超类。
            field.setAccessible(true);
            Object object = field.get(manager);
            System.out.println(object);//Hunk
        } catch (Exception e) {
            e.printStackTrace();
        }

        //Class对象最常用的方法是getName()，它将返回类的名字
        //如果类在一个包里，包的名字也作为类的一部分
        System.out.println(class1.getName() + " " + employee.getName());
        //com.hebostary.corejava.Employee Grace
        System.out.println(class2.getName() + " " + manager.getName());
        //com.hebostary.corejava.Manager Hunk

        //2. 调用Class的静态方法forName获得类名对应的Class对象
        //只有className是类名或接口时才能执行
        //无论何时使用这个方法，都应该提供异常处理器
        String className = "com.hebostary.corejava.Manager";
        try {
            Class class3 = Class.forName(className);

            //可以通过Class对象的newInstance()方法动态地创建类实例：
            //newInstance()方法调用默认的构造器初始化新创建的对象
            //如果这个类没有默认的构造器，就会抛出一个异常
            Employee employee2 = (Employee) class3.newInstance();
            System.out.println(employee2.getName());//NoneName

            //虚拟机为每个类型管理一个Class对象，因此，可以利用==运算符
            //比较两个类对象
            System.out.println(class2 == class3);//true
        } catch (Exception e) {
            System.out.println(e);
        }

        //3. T是任意的Java类型（或void关键字），T.class将代表匹配的类对象
        //注意，一个Class对象实际上表示的是一个类型，而这个类型不一定是类
        //也可以是int这种基本类型
        Class class4 = int.class;
        Class class5 = Manager.class;
        System.out.println(class4.getName());//int
        System.out.println(class5.getName());

        //Class类实际上是一个泛型类：
        //Manager.class的类型是Class<Manager>。
    }

    //打印类的所有构造器，方法和域信息
    public static void printMetadataOfClass() {
        Scanner in = new Scanner(System.in);
        System.out.println("Enter class name (e.g. com.hebostary.corejava.Manager):");
        String name = in.next();

        try {
            //打印类和超类的名字
            Class cl = Class.forName(name);
            Class superCl = cl.getSuperclass();
            String modifiers = Modifier.toString(cl.getModifiers());
            if (modifiers.length() > 0) System.out.print(modifiers + " ");
            System.out.print("class "+ name);
            
            if (superCl != null && superCl != Object.class) 
            {
                System.out.print(" extends " + superCl.getName());
            }

            System.out.print("\n{");
            printConstructors(cl);

            System.out.println();
            printMethods(cl);

            System.out.println();
            printFiels(cl);
            System.out.println("\n}");
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
    }
    /**
     * 1.打印一个类的所有构造器方法
    */
    public static void printConstructors(Class cl) {
        //获取类中声明的全部构造器，其中包括私有和受保护成员，但不包括超类成员
        Constructor[] constructors = cl.getDeclaredConstructors();

        for (Constructor constructor : constructors) {
            System.out.print("\n    ");
            //打印构造器的访问修饰符
            String modifiers = Modifier.toString(constructor.getModifiers());
            if (modifiers.length() > 0) System.out.print(modifiers + " ");
            System.out.print(constructor.getName() + "(");

            //打印参数类型
            Class[] paramTypes = constructor.getParameterTypes();
            for (int i = 0; i < paramTypes.length; i++) {
                if (i > 0) System.out.print(",");
                System.out.print(paramTypes[i].getName());
            }
            System.out.print(");");
        }
    }

    /**
     * 2.打印一个类的所有方法
    */
    public static void printMethods(Class cl) {
        //获取类中声明的全部方法，其中包括私有和受保护成员，但不包括超类成员
        Method[] methods = cl.getDeclaredMethods();

        for (Method method : methods) {
            Class returnType = method.getReturnType();

            System.out.print("\n    ");
            String modifiers = Modifier.toString(method.getModifiers());
            if (modifiers.length() > 0) System.out.print(modifiers + " ");
            System.out.print(returnType.getName() + " " + method.getName() + "(");

            //打印参数类型
            Class[] paramTypes = method.getParameterTypes();
            for (int i = 0; i < paramTypes.length; i++) {
                if (i > 0) System.out.print(",");
                System.out.print(paramTypes[i].getName());
            }
            System.out.print(");");
        }
    }

    /**
     * 3.打印类的所有域
    */
    public static void printFiels(Class cl) {
        //获取类中声明的全部域，其中包括私有和受保护成员，但不包括超类成员
        Field[] fields = cl.getDeclaredFields();

        for (Field field : fields) {
            //获取域的Class对象
            Class type = field.getType();
            System.out.print("\n    ");
            String modifiers = Modifier.toString(field.getModifiers());
            if (modifiers.length() > 0) System.out.print(modifiers + " ");
            System.out.print(type.getName() + " " + field.getName() + ";");
        }
    }

    public static void Test() {
        Common.PrintHeader("Reflection");
        testClass();

        printMetadataOfClass();

        System.exit(0);
    }
}