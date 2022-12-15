import subprocess


from exchangelib import Credentials
from exchangelib import DELEGATE, IMPERSONATION, Account
from exchangelib import Message, Mailbox
from exchangelib import FileAttachment

import time
import datetime

# if for some reason this script is still running
# after a year, we'll stop after 365 days
for i in range(0,365):
    # sleep until 2AM
    t = datetime.datetime.today()
    future = datetime.datetime(t.year,t.month,t.day,15,16)
    if t.hour >= 15 and t.minute >=16:
        future += datetime.timedelta(days=1)
    print(f'sleep {(future-t).total_seconds()} seconds')
    time.sleep((future-t).total_seconds())
    subprocess.run(["./export.sh"])

    credentials = Credentials(username='user', password='password')
    my_account = Account(
        primary_smtp_address='i@yandex.ru', credentials=credentials,
        autodiscover=True, access_type=DELEGATE
        )

    m = Message(
        account=my_account,
        subject='Отчет о инициативах'.encode('utf-8'),
        body='',
        to_recipients=[
            Mailbox(email_address='i@yandex.ru')
        ],  # Or a mix of both
    )
    m.attach(FileAttachment(
        name='readme.md', content=open('readme.md', 'rb').read())
    )
    m.send()