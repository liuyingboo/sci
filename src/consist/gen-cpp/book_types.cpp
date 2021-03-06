/**
 * Autogenerated by Thrift Compiler (0.9.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "book_types.h"

#include <algorithm>

namespace example {

const char* Book_Info::ascii_fingerprint = "5336EE826747522DE4E4045EA4440B86";
const uint8_t Book_Info::binary_fingerprint[16] = {0x53,0x36,0xEE,0x82,0x67,0x47,0x52,0x2D,0xE4,0xE4,0x04,0x5E,0xA4,0x44,0x0B,0x86};

uint32_t Book_Info::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->book_id);
          this->__isset.book_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->book_name);
          this->__isset.book_name = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->book_author);
          this->__isset.book_author = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_DOUBLE) {
          xfer += iprot->readDouble(this->book_price);
          this->__isset.book_price = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->book_publisher);
          this->__isset.book_publisher = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t Book_Info::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("Book_Info");

  xfer += oprot->writeFieldBegin("book_id", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32(this->book_id);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("book_name", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(this->book_name);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("book_author", ::apache::thrift::protocol::T_STRING, 3);
  xfer += oprot->writeString(this->book_author);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("book_price", ::apache::thrift::protocol::T_DOUBLE, 4);
  xfer += oprot->writeDouble(this->book_price);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("book_publisher", ::apache::thrift::protocol::T_STRING, 5);
  xfer += oprot->writeString(this->book_publisher);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(Book_Info &a, Book_Info &b) {
  using ::std::swap;
  swap(a.book_id, b.book_id);
  swap(a.book_name, b.book_name);
  swap(a.book_author, b.book_author);
  swap(a.book_price, b.book_price);
  swap(a.book_publisher, b.book_publisher);
  swap(a.__isset, b.__isset);
}

} // namespace
