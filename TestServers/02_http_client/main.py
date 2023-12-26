import urllib.request

req = urllib.request.Request('http://localhost:8000/')

with urllib.request.urlopen(req) as response:
   
   the_page = response.read()

   print(type(the_page))
   print(the_page.decode())