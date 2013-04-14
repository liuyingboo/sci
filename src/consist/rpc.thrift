namespace cpp example
include "book.thrift"
service BookServlet{
bool Sender(1:list<book.Book_Info> books);
oneway void Send2(1:list<book.Book_Info> books);
}