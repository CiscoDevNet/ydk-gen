package providers

import "testing"

const testHome = "."

func TestNetconfServiceProvider_Connect(t *testing.T) {
	provider := NetconfServiceProvider{Address: "127.0.0.1", Username: "admin", Password: "admin", Port: 12022}
	provider.Connect()
}

func TestNetconfServiceProvider_Disconnect(t *testing.T) {
	provider := NetconfServiceProvider{Address: "127.0.0.1", Username: "admin", Password: "admin", Port: 12022}
	provider.Disconnect()
}

func TestNetconfServiceProvider_GetPrivate(t *testing.T) {
	provider := NetconfServiceProvider{Address: "127.0.0.1", Username: "admin", Password: "admin", Port: 12022}
	provider.GetPrivate()
}

func TestRestconfServiceProvider_Connect(t *testing.T) {
	provider := RestconfServiceProvider{Path: testHome, Address: "localhost", Username: "admin", Password: "admin", Port: 12306}
	provider.Connect()
}

func TestRestconfServiceProvider_Disconnect(t *testing.T) {
	provider := RestconfServiceProvider{Path: testHome, Address: "localhost", Username: "admin", Password: "admin", Port: 12306}
	provider.Disconnect()
}

func TestRestconfServiceProvider_GetPrivate(t *testing.T) {
	provider := RestconfServiceProvider{Path: testHome, Address: "localhost", Username: "admin", Password: "admin", Port: 12306}
	provider.GetPrivate()
}
