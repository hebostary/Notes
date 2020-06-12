import com.hebostary.corejava.*;
import com.hebostary.corejava.collection.BitSetTest;
import com.hebostary.corejava.collection.CollectionTest;
import com.hebostary.corejava.collection.EnumSetMapTest;
import com.hebostary.corejava.collection.MapTest;
import com.hebostary.corejava.innerClass.InnerClass;
import com.hebostary.corejava.innerClass.StaticInnerClass;
import com.hebostary.corejava.lambda.*;
import com.hebostary.corejava.reflect.*;
import com.hebostary.corejava.stackTrace.StackTrace;
import com.hebostary.corejava.proxy.*;
import com.hebostary.corejava.exception.*;
import com.hebostary.corejava.generic.GenericMethod;
import com.hebostary.corejava.generic.PairTest;
import com.hebostary.corejava.concurrence.*;

public class CoreJavaTest
{
    public static void main(String[] args) {
        //1.测试基础数据类型
        //HelloWorld.Test();

        //2.测试基础类编写
        //Employee.Test();

        //3.测试类继承
        //Manager.Test();

        //4.测试ArrayList
        //ArrayListTest.Test();

        //5.测试枚举类
        //EnumTest.Test();

        //6.测试反射
        //Reflection.Test();

        //7.测试Comparable接口
        //EmployeeSort.Test();

        //8.测试lambda表达式
        //Lambda.Test();

        //9.测试内部类
        //InnerClass.Test();
        //StaticInnerClass.Test();
    
        //10.测试代理对象
        //ProxyTest.Test();

        //11.测试异常
        //ExceptionTest.Test();
        //StackTrace.Test();

        //12.测试泛型编程（generic）
        //GenericMethod.Test();
        //PairTest.Test();

        //13.测试集合
        //CollectionTest.Test();
        //MapTest.Test();
        //EnumSetMapTest.Test();
        //BitSetTest.Test();

        //14.并发测试
        ThreadInterrupt.Test();
    }
}