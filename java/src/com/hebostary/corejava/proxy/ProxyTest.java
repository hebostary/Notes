package com.hebostary.corejava.proxy;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.util.Arrays;
import java.util.Random;

import com.hebostary.corejava.Common;

public class ProxyTest {
    public static void Test(){
        Common.PrintClassHeader(new Object(){}.getClass()
            .getEnclosingClass().toString());

        Object[] elements = new Object[1000];

        for (int i = 0; i < elements.length; i++) {
            //1.创建一个整型对象
            Integer value = i + 1;

            //2.创建一个用于处理value对象的调用处理器
            InvocationHandler handler = new TraceHandler(value);
            
            //3.构造实现指定接口的代理类的一个新实例
            //代理对象属于在运行时定义的类（比如 $Proxy0）
            //这个类也实现了Comparable接口
            //但是它的compareTo方法实际上是调用了代理对象处理器（handler）的invoke方法
            Object proxy = Proxy.newProxyInstance(
                null, //类加载器，null表示使用默认的类加载器
                new Class[] { Comparable.class }, //Class对象数组，每个元素都是需要实现的接口 
                handler //调用处理器
            );

            //4.在数组中填充代理对象，
            //所以 compareTo 调用了 TraceHander 类中的 invoke 方法。
            //这个方法打印出了方法名和参数， 
            //之后用包装好的 Integer 对象调用 compareTo
            elements[i] = proxy;
        }

        //创建一个随机整数
        Integer key = new Random().nextInt(elements.length) + 1;

        //对key执行二分查找
        int result = Arrays.binarySearch(elements, key);

        //打印查找结果
        if (result >= 0) System.out.println(result);
    }
}

/*输出：
500.compareTo(632)
750.compareTo(632)
625.compareTo(632)
687.compareTo(632)
656.compareTo(632)
640.compareTo(632)
632.compareTo(632)
631
*/

/**
 * 调用处理器：用于跟踪方法调用，先打印方法名和参数，然后再执行原来的方法
*/
class TraceHandler implements InvocationHandler
{
    private Object targeObject;

    public TraceHandler(Object t)
    {
        targeObject = t;
    }

    public Object invoke(Object proxy, Method m, Object[] args) throws Throwable
    {
        //打印隐式参数
        System.out.print(targeObject);

        //打印方法名字
        System.out.print("." + m.getName() + "(");
    
        //打印显式参数
        if (args != null)
        {
            for (int i = 0; i < args.length; i++) {
                System.out.print(args[i]);
                if (i < args.length - 1) System.out.print(",");
            }
        }
        System.out.println(")");

        //执行实际的方法
        return m.invoke(targeObject, args);
    }
}