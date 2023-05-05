package ydk

// #cgo CXXFLAGS: -g -std=c++11
// #cgo darwin LDFLAGS: -lydk -lxml2 -lxslt -lpcre -lssh -lssh_threads -lcurl -lc++
// #cgo linux LDFLAGS:  -fprofile-arcs -ftest-coverage --coverage -lydk -lxml2 -lxslt -lpcre -lssh -lssh_threads -lcurl -lstdc++ -lm -ldl
//#include <ydk/ydk.h>
import "C"
import "fmt"

// LogLevel indicates what level of logging to output
type LogLevel int

const (
	Debug LogLevel = iota
	Info
	Warning
	Error
)

// EnableLogging enables logging on the code that runs in C++
func EnableLogging(level LogLevel) {
	switch level {
	case Debug:
		C.EnableLogging(C.DEBUG)

	case Info:
		C.EnableLogging(C.INFO)

	case Warning:
		C.EnableLogging(C.WARNING)

	case Error:
		C.EnableLogging(C.ERROR)
	}
}

func YLogInfo(msg string){
	msg = fmt.Sprintf("[Go] %s", msg)
    C.YLogInfo(C.CString(msg));
}

func YLogDebug(msg string){
	msg = fmt.Sprintf("[Go] %s", msg)
    C.YLogDebug(C.CString(msg));
}

func YLogWarn(msg string){
	msg = fmt.Sprintf("[Go] %s", msg)
    C.YLogWarn(C.CString(msg));
}

func YLogError(msg string){
	msg = fmt.Sprintf("[Go] %s", msg)
    C.YLogError(C.CString(msg));
}
