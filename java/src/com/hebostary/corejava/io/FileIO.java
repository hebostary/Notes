package com.hebostary.corejava.io;

import java.io.FileInputStream;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

import com.hebostary.corejava.*;

public class FileIO {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());

        final String fileString = "./employees.txt";
        ArrayList<Employee> employees = new ArrayList<>();
        employees.add(new Employee("Hunk", 10000));
        employees.add(new Employee("Grace", 9999));
        employees.add(new Employee("Jack", 11100));

        System.out.println("Begin to write employees to " + fileString);
        try (PrintWriter pWriter = new PrintWriter(fileString, "UTF-8")) {
            writeEmployee(employees, pWriter);
        } catch (Exception e) {
            e.printStackTrace();
        }

        System.out.println("Begin to read employees from " + fileString);
        try (Scanner in = new Scanner(new FileInputStream(fileString), "UTF-8")) {
            List<Employee> result = readEmployee(in);
            for (Employee employee : result) {
                System.out.println(employee);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Write employee records to file
    */
    private static void writeEmployee(List<Employee> list, PrintWriter outWriter) {
        for (Employee employee : list) {
            outWriter.println(employee.getName() + "|" + employee.getSalary());
        }
    }

    /**
     * Read employee records from file
    */
    private static List<Employee> readEmployee(Scanner in) {
        ArrayList<Employee> result = new ArrayList<>();
        while (in.hasNextLine()) {
            String record = in.nextLine();
            String[] tokens = record.split("\\|");
            if (tokens.length == 2) {
                result.add(new Employee(tokens[0], Double.parseDouble(tokens[1])));
            } else {
                System.out.println("Error employee record: " + record);
            }
            
        }
        return result;
    }


}