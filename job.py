import subprocess
import sys

from exchangelib import Credentials
from exchangelib import DELEGATE, IMPERSONATION, Account
from exchangelib import Message, Mailbox
from exchangelib import FileAttachment

import time
import datetime

if len(sys.argv) < 4:
    print("Usage: python3 job.py email login password")
    sys.exit()

email = sys.argv[1]
login = sys.argv[2]
password = sys.argv[3]

print(f'email={email} user={login}')
# if for some reason this script is still running
# after a year, we'll stop after 365 days
for i in range(0,365):
    # sleep until 2AM
    t = datetime.datetime.today()

    future = datetime.datetime(t.year,t.month,t.day,2,0)
    if t.hour >= 2:
        future += datetime.timedelta(days=1)
    print(f'sleep {(future-t).total_seconds()} seconds')
    #time.sleep((future-t).total_seconds())
    subprocess.run(["/bin/sh","export.sh"])

    credentials = Credentials(username=login, password=password)
    my_account = Account(
        primary_smtp_address=email, credentials=credentials,
        autodiscover=True, access_type=DELEGATE
        )

    m = Message(
        account=my_account,
        subject='Отчет о инициативах',
        body='',
        to_recipients=[
            Mailbox(email_address=email)
        ],  # Or a mix of both
    )
    m.attach(FileAttachment(
        name='report_'+str(t.year)+str(t.month)+str(t.day)+'.xlsx', content=open('report.xlsx', 'rb').read())
    )
    m.send()