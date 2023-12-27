package main

import (
	"context"
	"fmt"
	"lib/db"

	"go.mongodb.org/mongo-driver/bson"
)

type Restaurant struct {
	Name         string
	RestaurantId string        `bson:"restaurant_id,omitempty"`
	Cuisine      string        `bson:"cuisine,omitempty"`
	Address      interface{}   `bson:"address,omitempty"`
	Borough      string        `bson:"borough,omitempty"`
	Grades       []interface{} `bson:"grades,omitempty"`
	AvgRating    string        `bson:"avg_rating,omitempty"`
}

type Address struct {
	Street string
	City   string
	State  string
}

type Student struct {
	FirstName string  `bson:"first_name,omitempty"`
	LastName  string  `bson:"last_name,omitempty"`
	Address   Address `bson:"inline"`
	Age       int
}

func Curd() {
	var dc db.DBComm = new(db.MongoDriverImpl)
	if err := dc.Connect(context.TODO(), URI_WITH_CRED); err != nil {
		panic(err)
	}
	defer dc.Close(context.TODO())

	// Drop test databases
	var test_colls = map[string][]string{
		"sample_restaurants": {"restaurants"},
		"school":             {"students"},
	}

	for db, colls := range test_colls {
		for _, coll := range colls {
			err := dc.DropColl(context.TODO(), db, coll)
			if err != nil {
				panic(err)
			}
		}
	}

	dc.SelectColl("sample_restaurants", "restaurants")

	newRestaurant := Restaurant{Name: "8282", Cuisine: "Korean"}
	// Document inserted with ID: ObjectID("64b4f33c930d2a47add46492")
	if err := dc.InsertOne(context.TODO(), newRestaurant); err != nil {
		panic(err)
	}

	newRestaurants := []interface{}{
		Restaurant{Name: "Rule of Thirds", Cuisine: "Japanese", AvgRating: "3.0"},
		Restaurant{Name: "Qingzhen", RestaurantId: "0002", Cuisine: "China", AvgRating: "3.2"},
	}
	// 2 documents inserted with IDs: ObjectID("..."), ObjectID("...")
	if err := dc.InsertMany(context.TODO(), newRestaurants); err != nil {
		panic(err)
	}

	filter := bson.D{{"name", "Qingzhen"}}
	update := bson.D{{"$set", bson.D{{"restaurant_id", "0003"}, {"avg_rating", "4.4"}}}}
	if err := dc.UpdateMany(context.TODO(), filter, update); err != nil {
		panic(err)
	}

	filter = bson.D{{"avg_rating", bson.D{{"$lt", "4.0"}}}}
	update = bson.D{{"$set", bson.D{{"avg_rating", "4.0"}}}}
	if err := dc.UpdateMany(context.TODO(), filter, update); err != nil {
		panic(err)
	}

	filter = bson.D{{"name", "8282"}}
	if err := dc.DeleteMany(context.TODO(), filter); err != nil {
		panic(err)
	}

	dc.SelectColl("school", "students")
	address1 := Address{"1 Lakewood Way", "Elwood City", "PA"}
	student1 := Student{FirstName: "Arthur", Address: address1, Age: 8}
	// Document inserted with ID: ObjectID("6448f4660b58ac851e6a1fc2")
	if err := dc.InsertOne(context.TODO(), student1); err != nil {
		panic(err)
	}
	/*
		The corresponding BSON representation looks like this:
		   {
		     "_id" : ObjectId("..."),
		     "first_name" : "Arthur",
		     "street" : "1 Lakewood Way",
		     "city" : "Elwood City",
		     "state" : "PA",
		     "age" : 8
		   }
	*/
	filter = bson.D{{"age", 8}}

	var student_bson bson.D
	if err := dc.FindOne(context.TODO(), filter, &student_bson); err != nil {
		panic(err)
	}
	// [{_id ObjectID("6448f4660b58ac851e6a1fc2")} {first_name Arthur} {street 1 Lakewood Way} {city Elwood City} {state PA} {age 8}]
	fmt.Println(student_bson)

	var student_struct Student
	if err := dc.FindOne(context.TODO(), filter, &student_struct); err != nil {
		panic(err)
	}
	fmt.Println(student_struct)                //{Arthur  {1 Lakewood Way Elwood City PA} 8}
	fmt.Println(student_struct.Address.Street) //1 Lakewood Way
	fmt.Println(student_struct.Address.City)   //lwood City
	fmt.Println(student_struct.Address.State)  //PA
}
