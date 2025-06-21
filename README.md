# AI-Powered Lead Clustering and Outreach System

This project automates lead grouping, personalized email outreach using AI, and integrates C++ and Python for workflow automation.

## Features
- Parses lead data from CSV and groups by status and industry (C++)
- Uses BFS graph clustering for industry-based lead grouping
- Generates personalized outreach emails with Google Gemini AI (Python)
- Automatically sends emails via Gmail SMTP
- Full automation: generates JSON leads, triggers Python email script

## Setup
1. Clone the repo
2. Create and activate a Python virtual environment
3. Install dependencies: `pip install python-dotenv`
                         `pip install google-generativeai`
4. Set up `.env` with:
   - `EMAIL_ADDRESS`
   - `EMAIL_PASSWORD`
   - `GEMINI_API_KEY`
     Inlcude your own email, app password (in EMAIL_PASSWORD) and Gemini API Key.

Include a `.csv` file with the columns - ID,Name,Email,Title,Industry,Stage,Company,Details
5. Run the C++ executable to generate leads and trigger emails

## Notes
- Keep `.env` private; it contains sensitive credentials.
- Make sure Python and required modules are installed in your environment.
- It uses `gemini-2.0-flash` by default, but that can be changed based on your requirements in the main python script
- The cpp code is fixed for the columns given in `indian_leads_updated_stages.csv`, make sure you update the internal cpp code if adding or removing columns
