#include <memory>
#include <iostream>

#include "TestSupport.hxx"
#include "resiprocate/UnknownParameterType.hxx"
#include "resiprocate/Uri.hxx"
#include "resiprocate/os/DataStream.hxx"
#include "resiprocate/os/Logger.hxx"

using namespace resip;
using namespace std;

#define RESIPROCATE_SUBSYSTEM Subsystem::TEST

int
main(int argc, char* argv[])
{
   Log::Level l = Log::DEBUG;
   Log::initialize(Log::COUT, l, argv[0]);
   
   {
      Uri uri("sip:[5f1b:df00:ce3e:e200:20:800:2b37:6426:121.12.131.12]");

      cerr << "!! " << uri.host() << endl;
      assert(uri.host() == "5f1b:df00:ce3e:e200:20:800:2b37:6426:121.12.131.12");
      cerr << "!! " << Data::from(uri) << endl;
      assert(Data::from(uri) == "sip:[5f1b:df00:ce3e:e200:20:800:2b37:6426:121.12.131.12]");
   }

   {
      Uri uri("sip:user@[5f1b:df00:ce3e:e200:20:800:2b37:6426:121.12.131.12]");

      cerr << "!! " << uri.host() << endl;
      assert(uri.host() == "5f1b:df00:ce3e:e200:20:800:2b37:6426:121.12.131.12");
      cerr << "!! " << Data::from(uri) << endl;
      assert(Data::from(uri) == "sip:user@[5f1b:df00:ce3e:e200:20:800:2b37:6426:121.12.131.12]");
   }
   
   {
      Uri uri("sips:192.168.2.12");

      assert(uri.scheme() == "sips");
      assert(uri.password() == "");
      assert(uri.userParameters() == "");
      assert(uri.host() == "192.168.2.12");
      assert(uri.port() == 0);
   }

   {
      Uri uri("sips:host.foo.com");
      assert(uri.scheme() == "sips");
      assert(uri.password() == "");
      assert(uri.userParameters() == "");
      assert(uri.host() == "host.foo.com");
      assert(uri.port() == 0);
   }

   {
      Uri uri("sip:user;x-v17:password@host.com:5555");

      cerr << "user!!" << uri.user() << endl;
      cerr << "password!!" << uri.password() << endl;
      cerr << "userParams!!" << uri.userParameters() << endl;

      assert(uri.scheme() == "sip");
      assert(uri.user() == "user;x-v17");
      assert(uri.password() == "password");
      assert(uri.userParameters() == "");
      assert(uri.host() == "host.com");
      assert(uri.port() == 5555);
   }

   {
      // test bad parses
      try
      {
         Uri("noscheme@foo.com:1202");
         assert(false);
      }
      catch (ParseBuffer::Exception& e)
      {
      }
   }

   {
      // test q comparison
      Uri w1("sip:wombat@192.168.2.221:5062;transport=Udp;q=1.0");
      Uri w2("sip:wombat@192.168.2.221:5063;transport=Udp;q=0.5");
      Uri w3("sip:wombat@192.168.2.221:5063;transport=Udp;q=0.5");
      Uri w4("sip:wombat@192.168.2.221:5063;transport=Udp");

      Uri::GreaterQ gtQ;

      assert(gtQ(w1, w2));
      assert(!gtQ(w2, w1));
      assert(!gtQ(w2, w3));
      assert(!gtQ(w3, w2));
      assert(!gtQ(w1, w4));
      assert(!gtQ(w4, w1));
      assert(gtQ(w4, w3));
   }
   
   {
      Uri w1("sip:wombat@192.168.2.221:5062;transport=Udp");
      Uri w2("sip:wombat@192.168.2.221:5063;transport=Udp");
      assert(w1 != w2);
      assert(w1 < w2);
   }
   {
      Uri tel("tel:+358-555-1234567;pOstd=pP2;isUb=1411");
      assert(tel.user() == "+358-555-1234567");

      assert(Data::from(tel) == "tel:+358-555-1234567;pOstd=pP2;isUb=1411");
   }

   {
      Uri tel("tel:+358-555-1234567;pOstd=pP2;isUb=1411");
      Uri sip(Uri::fromTel(tel, "company.com"));

      cerr << "!! " << Data::from(sip) << endl;
      assert(Data::from(sip) == "sip:+358-555-1234567;isub=1411;postd=pp2@company.com;user=phone");
   }

   {
      Uri tel("tel:+358-555-1234567;foo=bar;aaaa=baz;pOstd=pP2;isUb=1411");
      Uri sip(Uri::fromTel(tel, "company.com"));

      cerr << "!! " << Data::from(sip) << endl;
      assert(Data::from(sip) == "sip:+358-555-1234567;isub=1411;postd=pp2;aaaa=baz;foo=bar@company.com;user=phone");
   }
   
   {
      Uri tel("tel:+358-555-1234567;postd=pp22");
      Uri sip(Uri::fromTel(tel, "foo.com"));
      assert(Data::from(sip) == "sip:+358-555-1234567;postd=pp22@foo.com;user=phone");
   }
   {
      Uri tel1("tel:+358-555-1234567;postd=pp22");
      Uri tel2("tel:+358-555-1234567;POSTD=PP22");
      cerr << "tel1=" << tel1 << " user=" << tel1.user() << endl;
      cerr << "tel2=" << tel2 << " user=" << tel2.user() << endl;
      assert (tel1 == tel2);
   }
   {
      Uri tel1("sip:+358-555-1234567;postd=pp22@foo.com;user=phone");
      Uri tel2("sip:+358-555-1234567;POSTD=PP22@foo.com;user=phone");
      assert (tel1 != tel2);
   }
   {
      Uri tel1("tel:+358-555-1234567;postd=pp22;isub=1411");
      Uri tel2("tel:+358-555-1234567;isub=1411;postd=pp22");
      // requires us to parse the user parameters
      //assert (tel1 == tel2);
   }
   {
      Uri tel1("sip:+358-555-1234567;postd=pp22;isub=1411@foo.com;user=phone");
      Uri tel2("sip:+358-555-1234567;isub=1411;postd=pp22@foo.com;user=phone");
      assert (tel1 != tel2);
   }
   {
      Uri tel1("tel:+358-555-1234567;postd=pp22");
      Uri tel2("tel:+358-555-1234567;POSTD=PP22");
      Uri sip1(Uri::fromTel(tel1, "foo.com"));
      Uri sip2(Uri::fromTel(tel2, "foo.com"));
      assert (sip1 == sip2);
      assert (Data::from(sip1) == "sip:+358-555-1234567;postd=pp22@foo.com;user=phone");
      assert (Data::from(sip2) == "sip:+358-555-1234567;postd=pp22@foo.com;user=phone");
   }
   {
      Uri tel1("tel:+358-555-1234567;tsp=a.b;phone-context=5");
      Uri tel2("tel:+358-555-1234567;phone-context=5;tsp=a.b");
      Uri sip1(Uri::fromTel(tel1, "foo.com"));
      Uri sip2(Uri::fromTel(tel2, "foo.com"));
      assert (sip1 == sip2);
      assert (Data::from(sip1) == "sip:+358-555-1234567;phone-context=5;tsp=a.b@foo.com;user=phone");
      assert (Data::from(sip2) == "sip:+358-555-1234567;phone-context=5;tsp=a.b@foo.com;user=phone");
   }

   {
      Uri uri("sip:fluffy@iii.ca:666");
      assert(uri.scheme() == "sip");
      assert(uri.user() == "fluffy");
      assert(uri.host() == "iii.ca");
      assert(uri.port() == 666);
   }
   
   {
      Uri uri("sip:fluffy@iii.ca;transport=tcp");
      assert(uri.param(p_transport) == "tcp");
   }
   
   {
      Uri uri("sips:fluffy@iii.ca;transport=tls");
      assert(uri.scheme() == "sips");
      assert(uri.param(p_transport) == "tls");
   }
   
   {
      Uri uri("sip:fluffy@iii.ca;transport=sctp");
      assert(uri.param(p_transport) == "sctp");
   }
   
   {
      Uri uri("sip:fluffy:password@iii.ca");
      assert(uri.password() == "password");
   }

   {
      Uri uri("sip:fluffy@iii.ca;user=phone;ttl=5;lr;maddr=1.2.3.4");
      assert(uri.param(p_ttl) == 5);
      assert(uri.exists(p_lr) == true);
      assert(uri.param(p_maddr) == "1.2.3.4");
      assert(uri.param(p_user) == "phone");
   }
 
   {
      Uri uri("sip:fluffy@iii.ca;x-fluffy=foo");
      assert(uri.exists(UnknownParameterType("x-fluffy")) == true);
      assert(uri.exists(UnknownParameterType("x-fufu")) == false);
      assert(uri.param(UnknownParameterType("x-fluffy")) == "foo");
   }
 
   {
      Uri uri("sip:fluffy@iii.ca;method=MESSAGE");
      assert(uri.param(p_method) == "MESSAGE");
   }

   {
      Uri uri("sip:+1(408) 444-1212:666@gw1");
      assert(uri.user() == "+1(408) 444-1212");
      assert(uri.password() == "666");
      assert(uri.host() == "gw1");
   }
 
   {
      Uri uri("sip:fluffy;x-utag=foo@iii.ca");
      assert(uri.user() == "fluffy;x-utag=foo");
      assert(uri.host() == "iii.ca");

      Data out(Data::from(uri));
      assert(out == "sip:fluffy;x-utag=foo@iii.ca");
   }

   {
      Uri uri("sip:fluffy;x-utag=foo:password@iii.ca");
      assert(uri.user() == "fluffy;x-utag=foo");
      assert(uri.host() == "iii.ca");
      assert(uri.password() == "password");

      Data out(Data::from(uri));
      cerr << "!! " << out << endl;
      assert(out == "sip:fluffy;x-utag=foo:password@iii.ca");
   }

   {
      Uri uri("tel:+14086661212");
      assert(uri.user() == "+14086661212");
      assert(uri.userParameters() == "");
      assert(uri.host() == "");
      assert(uri.password() == "");

      Data out(Data::from(uri));
      cerr << "!! " << out << endl;
      assert(out == "tel:+14086661212");
   }

   {
      Uri uri("tel:+14086661212;foo=bie");
      assert(uri.user() == "+14086661212");
      assert(uri.userParameters() == "foo=bie");
      assert(uri.host() == "");
      assert(uri.password() == "");

      Data out(Data::from(uri));
      cerr << "!! " << out << endl;
      assert(out == "tel:+14086661212;foo=bie");
   }

   {
      Uri uri("tel:+14086661212;");
      assert(uri.user() == "+14086661212");
      assert(uri.userParameters() == "");
      assert(uri.host() == "");
      assert(uri.password() == "");

      Data out(Data::from(uri));
      cerr << "!! " << out << endl;
      assert(out == "tel:+14086661212");
   }

   {
      Uri uri("sip:;:@");
      cerr << "uri.user() = " << uri.user() << endl;
      assert(uri.user() == ";");
      assert(uri.userParameters() == "");
      assert(uri.host() == "");
      assert(uri.password() == "");

      Data out(Data::from(uri));
      cerr << "!! " << out << endl;
      assert(out == "sip:;");
   }

   {
      Uri uri("tel:+1 (408) 555-1212");
      assert(uri.scheme() == "tel");
   }
   // Tests for user-less uris (was broken accidentally v1.44 Uri.cxx)
   {
     Data original("sip:1.2.3.4:5060");
     Data encoded;
     Uri uri(original);

     DataStream ds(encoded);
     uri.encode(ds);
     ds.flush();
     cout << "!! original data: " << original << endl;
     cout << "!! original uri : " << uri << endl;
     cout << "!! encoded  data: " << encoded << endl;

     assert( encoded == original );
   }
   {
      // Test order irrelevance of unknown parameters
      Uri sip1("sip:user@domain;foo=bar;baz=qux");
      Uri sip2("sip:user@domain;baz=qux;foo=bar");
      assert (sip1 == sip2);
   }
   cerr << endl << "All OK" << endl;
   return 0;
}
/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */
