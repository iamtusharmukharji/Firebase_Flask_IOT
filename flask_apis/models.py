from sqlalchemy import Column, VARCHAR, INTEGER, DATETIME ,create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
import os

engine = create_engine("sqlite:///flask_apis/database.db", echo=True)

base = declarative_base()

start_session = sessionmaker(bind=engine, autoflush=True)


# < --Schemas-- >
class Button(base):
    __tablename__ = "button"
    button_id = Column(INTEGER, primary_key = True, index =True)
    button_tag = Column(VARCHAR(200))
    state = Column(INTEGER)
    last_update = Column(DATETIME)



if not os.path.exists("database.db"):
    base.metadata.create_all(engine)