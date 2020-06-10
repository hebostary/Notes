package com.hebostary.corejava.collection;

import java.util.Collection;
import java.util.HashMap;
import java.util.IdentityHashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;

import com.hebostary.corejava.*;

public class MapTest {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        HashMapTest();
        LinkedHashMapTest();
        IdentityHashMapTest();
    }

    /**
     * 散列映射
    */
    public static void HashMapTest() {
        Common.PrintMethodHeader("HashMapTest");
        Map<String, Employee> staff = new HashMap<>();
        staff.put("2020-25-1", new Employee("Hunk", 10000));
        staff.put("2020-25-2", new Employee("Jack", 7800));
        staff.put("2020-25-3", new Employee("Grace", 56000));

        //打印所有条目
        System.out.println(staff);

        //移除一个条目
        staff.remove("2020-25-2");

        //替换条目
        staff.replace("2020-25-3", new Employee("Jim", 6000));

        //根据键查找
        System.out.println(staff.get("2020-25-3"));

        //映射的集合视图1：键集合
        //返回映射中所有键的一个集视图。可以从这个集中删除元素，键和相关联的值将从映
        //射中删除， 但是不能增加任何元素。
        Set<String> keys = staff.keySet();
        System.out.println(keys);

        //映射的集合视图2：值集合
        //返回映射中所有值的一个集合视图
        Collection<Employee> values = staff.values();
        System.out.println(values);

        //映射的集合视图3：键/值集合
        //返回 Map.Entry对象（映射中的键 / 值对）的一个集视图。可以从这个集中删除元素，
        //它们将从映射中删除，但是不能增加任何元素
        for (Map.Entry<String, Employee> entry : staff.entrySet()) {
            System.out.println("key=" + entry.getKey() 
            + ", value=" + entry.getValue());
        }

        //遍历条目最高效的方法：
        staff.forEach((k, v) -> 
        System.out.println("key=" + k + ", value=" + v));
    }

    /**
     * 链接散列映射：可以记住插入元素项的顺序
    */
    public static void LinkedHashMapTest() {
        Common.PrintMethodHeader("LinkedHashMapTest");
        Map<String, Employee> staff = new LinkedHashMap<>();
        staff.put("2020-25-1", new Employee("Hunk", 10000));
        staff.put("2020-25-2", new Employee("Jack", 7800));
        staff.put("2020-25-3", new Employee("Grace", 56000));

        //以插入顺序访问元素的键
        Iterator<String> iter = staff.keySet().iterator();
        while (iter.hasNext()) {
            System.out.println(iter.next());
            /*
            2020-25-1
            2020-25-2
            2020-25-3*/
        }

        //用访问顺序而不是插入顺序堆映射条目进行迭代
        //每次调用 get 或put, 受到影响的条目将从当前的位置删除， 
        //并放到条目链表的尾部（只有条目在链表中的位
        //置会受影响， 而散列表中的桶不会受影响。
        staff = new LinkedHashMap<>(8, 0.75F, true);
        staff.put("2020-25-1", new Employee("Hunk", 10000));
        staff.put("2020-25-2", new Employee("Jack", 7800));
        staff.put("2020-25-3", new Employee("Grace", 56000));
        System.out.println(staff.get("2020-25-2"));
        iter = staff.keySet().iterator();
        while (iter.hasNext()) {
            System.out.println(iter.next());
            /*
            2020-25-1
            2020-25-3
            2020-25-2*/
        }

        //TODO: debug the error
        /*
        Map<String, Employee> cache = new LinkedHashMap<>(8, 0.75F, true) 
        {
            protected boolean removeEldestEntry(Map.Entry<String, Employee> eldest)
            {
                return size() > 5;
            }
        }();

        cache.put("2020-25-1", new Employee("Hunk", 10000));
        cache.put("2020-25-2", new Employee("Jack", 7800));
        cache.put("2020-25-3", new Employee("Grace", 56000));
        System.out.println(cache.get("2020-25-2"));
        iter = cache.keySet().iterator();
        while (iter.hasNext()) {
            System.out.println(iter.next());
        }
        */
    }

    /**
     * IdentityHashMap比较键值是通过==比价的，而不是hashCode()的值
     * 实际上比较的是key的地址，因此可以存放键值相同的元素。
    */
    public static void IdentityHashMapTest() {
        Common.PrintMethodHeader("IdentityHashMapTest");
        Map<String, Integer> identityHashMap = new IdentityHashMap<>();
        identityHashMap.put(new String("Hunk"), 1000);
        identityHashMap.put(new String("Hunk"), 2000);
        System.out.println(identityHashMap.size());//2
        System.out.println(identityHashMap.get("Hunk"));//null
        identityHashMap.put("Hunk", 2000);
        identityHashMap.put("Hunk", 5000);
        System.out.println(identityHashMap.size());//3
        System.out.println(identityHashMap.get("Hunk"));//5000
        String name =  new String("Jack");
        identityHashMap.put(name, 8000);
        System.out.println(identityHashMap.get(name));//8000
    }

}