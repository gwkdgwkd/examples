package main

import (
	"fmt"
	"log"
	"net/http"
)

// type Handler interface {
// 	ServeHTTP(w ResponseWriter, r *Request)
// }
// func ListenAndServe(address string, h Handler) error

// ListenAndServe函数需要一个例如“localhost:8000”的服务器地址,和一个所有请求都可以分
// 派的Handler接口实例。它会一直运行,直到这个服务因为一个错误而失败(或者启动失
// 败),它的返回值一定是一个非空的错误。

type dollars float32

func (d dollars) String() string { return fmt.Sprintf("$%.2f", d) }

type database map[string]dollars

func (db database) list(w http.ResponseWriter, req *http.Request) {
	for item, price := range db {
		fmt.Fprintf(w, "%s:	%s\n", item, price)
	}
}
func (db database) price(w http.ResponseWriter, req *http.Request) {
	item := req.URL.Query().Get("item")
	price, ok := db[item]
	if !ok {
		w.WriteHeader(http.StatusNotFound) // 404
		fmt.Fprintf(w, "no such item: %q\n", item)
		return
	}
	fmt.Fprintf(w, "%s\n", price)
}

func (db database) ServeHTTP(w http.ResponseWriter, req *http.Request) {
	switch req.URL.Path {
	case "/list":
		for item, price := range db {
			fmt.Fprintf(w, "%s:	%s\n", item, price)
		}
	case "/price":
		item := req.URL.Query().Get("item")
		price, ok := db[item]
		if !ok {
			w.WriteHeader(http.StatusNotFound) // 404
			fmt.Fprintf(w, "no such	item: %q\n", item)
			return
		}
		fmt.Fprintf(w, "%s\n", price)
	default:
		w.WriteHeader(http.StatusNotFound) // 404
		fmt.Fprintf(w, "no such	page: %s\n", req.URL)
	}
}
func main() {
	db := database{"shoes": 50, "socks": 5}
	// log.Fatal(http.ListenAndServe("localhost:8000", db))

	// net/http包提供了一个请求多路器ServeMux来简化URL和handlers的联系。
	// 一个ServeMux将一批http.Handler聚集到一个单一的http.Handler中。
	// 再一次,我们可以看到满足同一接口的不同类型是可替换的:web服务器将请求指
	// 派给任意的http.Handler而不需要考虑它后面的具体类型。
	// mux := http.NewServeMux()
	// mux.Handle("/list", http.HandlerFunc(db.list))
	// mux.Handle("/price", http.HandlerFunc(db.price))
	// log.Fatal(http.ListenAndServe("localhost:8000", mux))
	// 语句http.HandlerFunc(db.list)是一个转换而非一个函数调用,因为http.HandlerFunc是一个类型。

	// Go语言目前没有一个权威的web框架,就像Ruby语言有Rails和python有Django。
	// 这并不是说这样的框架不存在,而是Go语言标准库中的构建模块就已经非常灵活以至
	// 于这些框架都是不必要的。此外,尽管在一个项目早期使用框架是非常方便的,但是它
	// 们带来额外的复杂度会使长期的维护更加困难。

	// ServeMux有一个方便的HandleFunc方法,它帮我们简化handler注册代码成这样:
	http.HandleFunc("/list", db.list)
	http.HandleFunc("/price", db.price)
	// 为了方便,net/http包提供了一个全局的ServeMux实例DefaultServerMux和包级别的
	// http.Handle和http.HandleFunc函数。现在,为了使用DefaultServeMux作为服务器的主
	// handler,不需要将它传给ListenAndServe函数;nil值就可以工作。
	log.Fatal(http.ListenAndServe("localhost:8000", nil))
}
