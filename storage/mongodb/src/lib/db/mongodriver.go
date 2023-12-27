package db

import (
	"context"
	"fmt"
	"lib/log"

	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
)

type MongoDriverImpl struct {
	client *mongo.Client
	coll   *mongo.Collection
}

func (m *MongoDriverImpl) Connect(ctx context.Context, server string) (err error) {
	log.Info("Connecting to " + server)
	// Use the SetServerAPIOptions() method to set the Stable API version to 1
	serverAPI := options.ServerAPI(options.ServerAPIVersion1)
	opts := options.Client().ApplyURI(server).SetServerAPIOptions(serverAPI)
	// Create a new client and connect to the server
	m.client, err = mongo.Connect(ctx, opts)
	if err != nil {
		log.Error(err.Error())
		return err
	}

	// Send a ping to confirm a successful connection
	var result bson.M
	if err := m.client.Database("admin").RunCommand(context.TODO(), bson.D{{"ping", 1}}).Decode(&result); err != nil {
		log.Error(err.Error())
		return err
	}

	log.Info("Successfully connected to MongoDB!")
	return nil
}

func (m *MongoDriverImpl) Close(ctx context.Context) (err error) {
	if err := m.client.Disconnect(ctx); err != nil {
		log.Error(err.Error())
		return err
	}
	log.Info("Disconnected from MongoDB!")
	return nil
}

func (m *MongoDriverImpl) SelectColl(db string, coll string) (err error) {
	m.coll = m.client.Database(db).Collection(coll)
	return nil
}

func (m *MongoDriverImpl) DropColl(ctx context.Context, db string, coll string) (err error) {
	err = m.client.Database(db).Collection(coll).Drop(ctx)
	if err != nil {
		log.Error(err.Error())
		return err
	}
	log.Info("Successfully droped collection " + db + "." + coll)
	return nil
}

func (m *MongoDriverImpl) InsertOne(ctx context.Context, doc interface{}) (err error) {
	result, err := m.coll.InsertOne(ctx, doc)
	if err != nil {
		log.Error(err.Error())
		return err
	}

	log.Info(fmt.Sprintf("Document inserted with ID: %s\n", result.InsertedID))
	return nil
}

func (m *MongoDriverImpl) InsertMany(ctx context.Context, docs []interface{}) (err error) {
	results, err := m.coll.InsertMany(ctx, docs)
	if err != nil {
		log.Error(err.Error())
		return err
	}

	log.Info(fmt.Sprintf("%d documents inserted with IDs:\n", len(results.InsertedIDs)))
	for _, id := range results.InsertedIDs {
		log.Info(fmt.Sprintf("\t%s\n", id))
	}
	return nil
}

func (m *MongoDriverImpl) UpdateMany(ctx context.Context, filter interface{}, update interface{}) (err error) {
	result, err := m.coll.UpdateMany(ctx, filter, update)
	if err != nil {
		log.Error(err.Error())
		return err
	}

	log.Info(fmt.Sprintf("Documents updated: %v\n", result.ModifiedCount))
	return nil
}

func (m *MongoDriverImpl) DeleteMany(ctx context.Context, filter interface{}) (err error) {
	result, err := m.coll.DeleteMany(ctx, filter)
	if err != nil {
		log.Error(err.Error())
		return err
	}

	log.Info(fmt.Sprintf("Documents deleted: %d\n", result.DeletedCount))
	return nil
}

func (m *MongoDriverImpl) FindOne(ctx context.Context, filter interface{}, result interface{}) (err error) {
	return m.coll.FindOne(ctx, filter).Decode(result)
}
