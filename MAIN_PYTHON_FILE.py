import json
from dotenv import load_dotenv
import os
import google.generativeai as genai
import smtplib
from email.mime.text import MIMEText

load_dotenv()

api_key = os.getenv("GEMINI_API_KEY")
genai.configure(api_key=api_key)

with open("industry_emails.json", "r") as file:
    data = json.load(file)

context = data["context"]
lead = data['lead']

prompt = f"""
Write a short, friendly outreach email to {lead['name']}, a {lead['title']} in the {lead['company']} company
in the{lead['industry']} industry.
The client is in a sales pipeline, currently at the stage {lead['stage']}.
Include a 2-3 line personalized icebreaker in the beginning. I'm giving you some details for context. These are details about other people that have been contacted that belong to the same industry as theirs. Use these details for context:
{context}

Here is some context about our company:
We are a B2B company (QuickAI Ltd.)
that helps businesses automate their workflows with the help of AI based workflows and integrations.
Mention that you've reached out to others in the same space.
Make it feel personal but professional.
Below are the entire lead details of the client
{lead}

Your output should only have the content of the email, nothing else at all.
The email will be sent from Rohan Singh, Founder and CEO, QuickAI Ltd.
Include no placeholders, as this mail will be sent directly without any formatting.
Hi Sneha,

I'm reaching out from QuickAI Ltd., where we specialize in helping businesses like yours streamline operations through AI-powered workflow automation. We've worked with others, particularly in the finance space, such as with Rosy from United Financial Services Ltd, and also with 2BitFinance Ltd.

From customer support to internal task handling, our integrations eliminate repetitive work and boost efficiency across teams — without the need for deep tech expertise.

Would you be open to a quick call this week to explore how we could automate parts of your current process and save both time and cost?

Looking forward to your response!

Best regards,
Rakesh,
Customer Relationship Manager,
QuickAI Ltd.
"""

model = genai.GenerativeModel("gemini-2.0-flash")
response = model.generate_content(prompt)

print("\nEmail has been generated\n")

email_sender = os.getenv("EMAIL_ADDRESS")
email_password = os.getenv("EMAIL_PASSWORD")
email_receiver = lead['email']

msg = MIMEText(response.text)
msg["Subject"] = f"Hey {lead['name']}! Free {lead['company']} from repetitive tasks"
msg["From"] = email_sender
msg["To"] = email_receiver

with smtplib.SMTP_SSL("smtp.gmail.com", 465) as smtp:
    smtp.login(email_sender, email_password)
    smtp.send_message(msg)

print(f"Email sent to {email_receiver}")
