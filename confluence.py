import requests
from requests.auth import HTTPBasicAuth
import json


auth = HTTPBasicAuth("", "")


headers = {
  "Accept": "application/json",
  "Content-Type": "application/json"
}


url = "https://confluence.mts.ru/rest/api/content/572453516?expand=body.storage"
response = requests.request(
   "GET",
   url,
   headers=headers,
   auth=auth
)
print(response.status_code)
print(response.text)