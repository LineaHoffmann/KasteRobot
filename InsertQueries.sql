# Insert Queries
# Get newst log_ID from log, based on created_at
SELECT log_ID FROM log WHERE created_at in(SELECT MAX(created_at) FROM log);

# Get newest, only works if more than one entry
SELECT * FROM log_XXXX WHERE created_at in(SELECT MAX(created_at) FROM log);