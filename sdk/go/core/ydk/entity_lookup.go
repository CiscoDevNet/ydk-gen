package ydk

import (
	"fmt"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"reflect"
)

var topEntityRegistry = make(map[string]reflect.Type)

// RegisterEntity
func RegisterEntity(name string, entity_type reflect.Type) {
	topEntityRegistry[name] = entity_type
}

// GetTopEntity
func GetTopEntity(name string) (types.Entity, bool) {
	_, ok := topEntityRegistry[name]
	if !ok {
                YLogError(fmt.Sprintf("Entity '%s' is not registered. Please import corresponding package to your application.", name))
                return nil, ok
	}
	return reflect.New(topEntityRegistry[name]).Elem().Addr().Interface().(types.Entity), ok
}
