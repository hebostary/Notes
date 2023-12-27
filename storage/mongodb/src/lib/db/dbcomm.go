package db

import (
	"context"
)

type DBComm interface {
	Connect(ctx context.Context, server string) (err error)
	Close(ctx context.Context) (err error)
	SelectColl(db string, coll string) (err error)
	DropColl(ctx context.Context, db string, coll string) (err error)
	InsertOne(ctx context.Context, doc interface{}) (err error)
	InsertMany(ctx context.Context, docs []interface{}) (err error)
	UpdateMany(ctx context.Context, filter interface{}, update interface{}) (err error)
	DeleteMany(ctx context.Context, filter interface{}) (err error)
	FindOne(ctx context.Context, filter interface{}, result interface{}) (err error)
}
