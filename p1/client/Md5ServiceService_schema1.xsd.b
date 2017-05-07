<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<xs:schema version="1.0" targetNamespace="http://ws/" xmlns:tns="http://ws/" xmlns:xs="http://www.w3.org/2001/XMLSchema">

  <xs:element name="Exception" type="tns:Exception"/>

  <xs:element name="md5" type="tns:md5"/>

  <xs:element name="md5Response" type="tns:md5Response"/>

  <xs:complexType name="md5">
    <xs:sequence>
      <xs:element name="arg0" type="xs:string" minOccurs="0"/>
    </xs:sequence>
  </xs:complexType>

  <xs:complexType name="md5Response">
    <xs:sequence>
      <xs:element name="return" type="xs:string" minOccurs="0"/>
    </xs:sequence>
  </xs:complexType>

  <xs:complexType name="Exception">
    <xs:sequence>
      <xs:element name="message" type="xs:string" minOccurs="0"/>
    </xs:sequence>
  </xs:complexType>
</xs:schema>

