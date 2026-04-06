from flask import Flask, request, jsonify
import os
import psycopg

app = Flask(__name__)
MAX_SCORES = 10

def get_conn():
    db_url = os.environ.get("DATABASE_URL")
    if not db_url:
        raise RuntimeError("DATABASE_URL env var is not set")
    return psycopg.connect(db_url, sslmode="require")

@app.route("/highscores", methods=["GET"])
def get_highscores():
    with get_conn() as conn:
        with conn.cursor() as cur:
            cur.execute(
                """
                select initials, score
                from public.highscores
                order by score desc, created_at asc
                limit %s;
                """,
                (MAX_SCORES,),
            )
            rows = cur.fetchall()

    return jsonify({"highscores": [{"initials": r[0], "score": r[1]} for r in rows]})

@app.route("/submit", methods=["POST"])
def submit_score():
    data = request.get_json(silent=True)
    if not data or "initials" not in data or "score" not in data:
        return {"error": "Invalid payload"}, 400

    initials = str(data["initials"])[:3].upper()
    try:
        score = int(data["score"])
    except Exception:
        return {"error": "Score must be an integer"}, 400

    if score < 0:
        return {"error": "Score must be >= 0"}, 400

    with get_conn() as conn:
        with conn.cursor() as cur:
            cur.execute(
                "insert into public.highscores (initials, score) values (%s, %s);",
                (initials, score),
            )
        conn.commit()

    return {"ok": True}

if __name__ == "__main__":
    port = int(os.environ.get("PORT", "5000"))
    app.run(host="0.0.0.0", port=port, debug=True)