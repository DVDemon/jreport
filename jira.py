import requests

r = requests.get('https://jira.mts.ru/rest/api/2/issue/KA-2229', auth=('', ''))
print(r.text)