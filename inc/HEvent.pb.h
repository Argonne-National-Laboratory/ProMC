// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: HEvent.proto

#ifndef PROTOBUF_HEvent_2eproto__INCLUDED
#define PROTOBUF_HEvent_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace promc {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_HEvent_2eproto();
void protobuf_AssignDesc_HEvent_2eproto();
void protobuf_ShutdownFile_HEvent_2eproto();

class HEvent;
class HEvent_Entry;
class HEvent_Group;

// ===================================================================

class HEvent_Entry : public ::google::protobuf::Message {
 public:
  HEvent_Entry();
  virtual ~HEvent_Entry();

  HEvent_Entry(const HEvent_Entry& from);

  inline HEvent_Entry& operator=(const HEvent_Entry& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const HEvent_Entry& default_instance();

  void Swap(HEvent_Entry* other);

  // implements Message ----------------------------------------------

  HEvent_Entry* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const HEvent_Entry& from);
  void MergeFrom(const HEvent_Entry& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string name = 1;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 1;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  inline void set_allocated_name(::std::string* name);

  // repeated double da = 3 [packed = true];
  inline int da_size() const;
  inline void clear_da();
  static const int kDaFieldNumber = 3;
  inline double da(int index) const;
  inline void set_da(int index, double value);
  inline void add_da(double value);
  inline const ::google::protobuf::RepeatedField< double >&
      da() const;
  inline ::google::protobuf::RepeatedField< double >*
      mutable_da();

  // repeated int64 ida = 2 [packed = true];
  inline int ida_size() const;
  inline void clear_ida();
  static const int kIdaFieldNumber = 2;
  inline ::google::protobuf::int64 ida(int index) const;
  inline void set_ida(int index, ::google::protobuf::int64 value);
  inline void add_ida(::google::protobuf::int64 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
      ida() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
      mutable_ida();

  // @@protoc_insertion_point(class_scope:promc.HEvent.Entry)
 private:
  inline void set_has_name();
  inline void clear_has_name();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* name_;
  ::google::protobuf::RepeatedField< double > da_;
  mutable int _da_cached_byte_size_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int64 > ida_;
  mutable int _ida_cached_byte_size_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];

  friend void  protobuf_AddDesc_HEvent_2eproto();
  friend void protobuf_AssignDesc_HEvent_2eproto();
  friend void protobuf_ShutdownFile_HEvent_2eproto();

  void InitAsDefaultInstance();
  static HEvent_Entry* default_instance_;
};
// -------------------------------------------------------------------

class HEvent_Group : public ::google::protobuf::Message {
 public:
  HEvent_Group();
  virtual ~HEvent_Group();

  HEvent_Group(const HEvent_Group& from);

  inline HEvent_Group& operator=(const HEvent_Group& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const HEvent_Group& default_instance();

  void Swap(HEvent_Group* other);

  // implements Message ----------------------------------------------

  HEvent_Group* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const HEvent_Group& from);
  void MergeFrom(const HEvent_Group& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string name = 1;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 1;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  inline void set_allocated_name(::std::string* name);

  // repeated .promc.HEvent.Entry entry = 2;
  inline int entry_size() const;
  inline void clear_entry();
  static const int kEntryFieldNumber = 2;
  inline const ::promc::HEvent_Entry& entry(int index) const;
  inline ::promc::HEvent_Entry* mutable_entry(int index);
  inline ::promc::HEvent_Entry* add_entry();
  inline const ::google::protobuf::RepeatedPtrField< ::promc::HEvent_Entry >&
      entry() const;
  inline ::google::protobuf::RepeatedPtrField< ::promc::HEvent_Entry >*
      mutable_entry();

  // @@protoc_insertion_point(class_scope:promc.HEvent.Group)
 private:
  inline void set_has_name();
  inline void clear_has_name();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* name_;
  ::google::protobuf::RepeatedPtrField< ::promc::HEvent_Entry > entry_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_HEvent_2eproto();
  friend void protobuf_AssignDesc_HEvent_2eproto();
  friend void protobuf_ShutdownFile_HEvent_2eproto();

  void InitAsDefaultInstance();
  static HEvent_Group* default_instance_;
};
// -------------------------------------------------------------------

class HEvent : public ::google::protobuf::Message {
 public:
  HEvent();
  virtual ~HEvent();

  HEvent(const HEvent& from);

  inline HEvent& operator=(const HEvent& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const HEvent& default_instance();

  void Swap(HEvent* other);

  // implements Message ----------------------------------------------

  HEvent* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const HEvent& from);
  void MergeFrom(const HEvent& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  typedef HEvent_Entry Entry;
  typedef HEvent_Group Group;

  // accessors -------------------------------------------------------

  // optional int64 id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::google::protobuf::int64 id() const;
  inline void set_id(::google::protobuf::int64 value);

  // optional string name = 2;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 2;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  inline void set_allocated_name(::std::string* name);

  // repeated double da = 3 [packed = true];
  inline int da_size() const;
  inline void clear_da();
  static const int kDaFieldNumber = 3;
  inline double da(int index) const;
  inline void set_da(int index, double value);
  inline void add_da(double value);
  inline const ::google::protobuf::RepeatedField< double >&
      da() const;
  inline ::google::protobuf::RepeatedField< double >*
      mutable_da();

  // repeated int64 ida = 4 [packed = true];
  inline int ida_size() const;
  inline void clear_ida();
  static const int kIdaFieldNumber = 4;
  inline ::google::protobuf::int64 ida(int index) const;
  inline void set_ida(int index, ::google::protobuf::int64 value);
  inline void add_ida(::google::protobuf::int64 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
      ida() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
      mutable_ida();

  // repeated .promc.HEvent.Group group = 5;
  inline int group_size() const;
  inline void clear_group();
  static const int kGroupFieldNumber = 5;
  inline const ::promc::HEvent_Group& group(int index) const;
  inline ::promc::HEvent_Group* mutable_group(int index);
  inline ::promc::HEvent_Group* add_group();
  inline const ::google::protobuf::RepeatedPtrField< ::promc::HEvent_Group >&
      group() const;
  inline ::google::protobuf::RepeatedPtrField< ::promc::HEvent_Group >*
      mutable_group();

  // @@protoc_insertion_point(class_scope:promc.HEvent)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_name();
  inline void clear_has_name();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int64 id_;
  ::std::string* name_;
  ::google::protobuf::RepeatedField< double > da_;
  mutable int _da_cached_byte_size_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int64 > ida_;
  mutable int _ida_cached_byte_size_;
  ::google::protobuf::RepeatedPtrField< ::promc::HEvent_Group > group_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];

  friend void  protobuf_AddDesc_HEvent_2eproto();
  friend void protobuf_AssignDesc_HEvent_2eproto();
  friend void protobuf_ShutdownFile_HEvent_2eproto();

  void InitAsDefaultInstance();
  static HEvent* default_instance_;
};
// ===================================================================


// ===================================================================

// HEvent_Entry

// optional string name = 1;
inline bool HEvent_Entry::has_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void HEvent_Entry::set_has_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void HEvent_Entry::clear_has_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void HEvent_Entry::clear_name() {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& HEvent_Entry::name() const {
  return *name_;
}
inline void HEvent_Entry::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void HEvent_Entry::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void HEvent_Entry::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* HEvent_Entry::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  return name_;
}
inline ::std::string* HEvent_Entry::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void HEvent_Entry::set_allocated_name(::std::string* name) {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    delete name_;
  }
  if (name) {
    set_has_name();
    name_ = name;
  } else {
    clear_has_name();
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// repeated double da = 3 [packed = true];
inline int HEvent_Entry::da_size() const {
  return da_.size();
}
inline void HEvent_Entry::clear_da() {
  da_.Clear();
}
inline double HEvent_Entry::da(int index) const {
  return da_.Get(index);
}
inline void HEvent_Entry::set_da(int index, double value) {
  da_.Set(index, value);
}
inline void HEvent_Entry::add_da(double value) {
  da_.Add(value);
}
inline const ::google::protobuf::RepeatedField< double >&
HEvent_Entry::da() const {
  return da_;
}
inline ::google::protobuf::RepeatedField< double >*
HEvent_Entry::mutable_da() {
  return &da_;
}

// repeated int64 ida = 2 [packed = true];
inline int HEvent_Entry::ida_size() const {
  return ida_.size();
}
inline void HEvent_Entry::clear_ida() {
  ida_.Clear();
}
inline ::google::protobuf::int64 HEvent_Entry::ida(int index) const {
  return ida_.Get(index);
}
inline void HEvent_Entry::set_ida(int index, ::google::protobuf::int64 value) {
  ida_.Set(index, value);
}
inline void HEvent_Entry::add_ida(::google::protobuf::int64 value) {
  ida_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
HEvent_Entry::ida() const {
  return ida_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
HEvent_Entry::mutable_ida() {
  return &ida_;
}

// -------------------------------------------------------------------

// HEvent_Group

// optional string name = 1;
inline bool HEvent_Group::has_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void HEvent_Group::set_has_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void HEvent_Group::clear_has_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void HEvent_Group::clear_name() {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& HEvent_Group::name() const {
  return *name_;
}
inline void HEvent_Group::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void HEvent_Group::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void HEvent_Group::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* HEvent_Group::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  return name_;
}
inline ::std::string* HEvent_Group::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void HEvent_Group::set_allocated_name(::std::string* name) {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    delete name_;
  }
  if (name) {
    set_has_name();
    name_ = name;
  } else {
    clear_has_name();
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// repeated .promc.HEvent.Entry entry = 2;
inline int HEvent_Group::entry_size() const {
  return entry_.size();
}
inline void HEvent_Group::clear_entry() {
  entry_.Clear();
}
inline const ::promc::HEvent_Entry& HEvent_Group::entry(int index) const {
  return entry_.Get(index);
}
inline ::promc::HEvent_Entry* HEvent_Group::mutable_entry(int index) {
  return entry_.Mutable(index);
}
inline ::promc::HEvent_Entry* HEvent_Group::add_entry() {
  return entry_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::promc::HEvent_Entry >&
HEvent_Group::entry() const {
  return entry_;
}
inline ::google::protobuf::RepeatedPtrField< ::promc::HEvent_Entry >*
HEvent_Group::mutable_entry() {
  return &entry_;
}

// -------------------------------------------------------------------

// HEvent

// optional int64 id = 1;
inline bool HEvent::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void HEvent::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void HEvent::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void HEvent::clear_id() {
  id_ = GOOGLE_LONGLONG(0);
  clear_has_id();
}
inline ::google::protobuf::int64 HEvent::id() const {
  return id_;
}
inline void HEvent::set_id(::google::protobuf::int64 value) {
  set_has_id();
  id_ = value;
}

// optional string name = 2;
inline bool HEvent::has_name() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void HEvent::set_has_name() {
  _has_bits_[0] |= 0x00000002u;
}
inline void HEvent::clear_has_name() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void HEvent::clear_name() {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& HEvent::name() const {
  return *name_;
}
inline void HEvent::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void HEvent::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void HEvent::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* HEvent::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  return name_;
}
inline ::std::string* HEvent::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void HEvent::set_allocated_name(::std::string* name) {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    delete name_;
  }
  if (name) {
    set_has_name();
    name_ = name;
  } else {
    clear_has_name();
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// repeated double da = 3 [packed = true];
inline int HEvent::da_size() const {
  return da_.size();
}
inline void HEvent::clear_da() {
  da_.Clear();
}
inline double HEvent::da(int index) const {
  return da_.Get(index);
}
inline void HEvent::set_da(int index, double value) {
  da_.Set(index, value);
}
inline void HEvent::add_da(double value) {
  da_.Add(value);
}
inline const ::google::protobuf::RepeatedField< double >&
HEvent::da() const {
  return da_;
}
inline ::google::protobuf::RepeatedField< double >*
HEvent::mutable_da() {
  return &da_;
}

// repeated int64 ida = 4 [packed = true];
inline int HEvent::ida_size() const {
  return ida_.size();
}
inline void HEvent::clear_ida() {
  ida_.Clear();
}
inline ::google::protobuf::int64 HEvent::ida(int index) const {
  return ida_.Get(index);
}
inline void HEvent::set_ida(int index, ::google::protobuf::int64 value) {
  ida_.Set(index, value);
}
inline void HEvent::add_ida(::google::protobuf::int64 value) {
  ida_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
HEvent::ida() const {
  return ida_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
HEvent::mutable_ida() {
  return &ida_;
}

// repeated .promc.HEvent.Group group = 5;
inline int HEvent::group_size() const {
  return group_.size();
}
inline void HEvent::clear_group() {
  group_.Clear();
}
inline const ::promc::HEvent_Group& HEvent::group(int index) const {
  return group_.Get(index);
}
inline ::promc::HEvent_Group* HEvent::mutable_group(int index) {
  return group_.Mutable(index);
}
inline ::promc::HEvent_Group* HEvent::add_group() {
  return group_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::promc::HEvent_Group >&
HEvent::group() const {
  return group_;
}
inline ::google::protobuf::RepeatedPtrField< ::promc::HEvent_Group >*
HEvent::mutable_group() {
  return &group_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace promc

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_HEvent_2eproto__INCLUDED
