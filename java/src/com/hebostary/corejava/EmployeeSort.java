package com.hebostary.corejava;

import java.util.Arrays;

public class EmployeeSort {
    public static void Test()
    {
        Common.PrintClassHeader(EmployeeSort.class.getName());
        Employee[] staff = new Employee[3];

        staff[0] = new Employee("Hunk", 10000);
        staff[1] = new Employee("Grace", 8000);
        staff[2] = new Employee("Alice", 5000);

        try {
            Arrays.sort(staff);
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        for (Employee employee : staff) 
        {
            System.out.println("name="+employee.getName() + 
            ", salary=" + employee.getSalary());
        }
    }
}