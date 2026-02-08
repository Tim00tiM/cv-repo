package repository

import (
	"database/sql"
	"encoding/json"
	"errors"
	"fmt"

	"fs-api/internal/model"
)

type Repository struct {
	DB *sql.DB
}

func (r *Repository) CreateUser(user *model.User) error {
	var id int
	err := r.DB.QueryRow(`INSERT INTO users (name, email, password_hash) VALUES ($1, $2, $3) RETURNING id`,
		user.Name, user.Email, user.PasswordHash).Scan(&id)

	user.ID = id
	return err
}

func (r *Repository) CheckCredentials(user *model.User) error {
	rows, err := r.DB.Query(`SELECT id, name, email FROM users WHERE users.email = $1 AND users.password_hash = $2`,
		user.Email, user.PasswordHash)
	if err != nil {
		return err
	}
	defer rows.Close()

	exist := rows.Next()
	if !exist {
		return fmt.Errorf("credentials are incorrect")
	}
	var u model.User
	if err := rows.Scan(&u.ID, &u.Name, &u.Email); err != nil {
		return err
	}
	*user = u
	return nil
}

func (r *Repository) GetUserInfo(userID int) (*model.User, error) {
	rows, err := r.DB.Query(`
		SELECT id, name, email FROM users WHERE id=$1`, userID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	exist := rows.Next()
	if !exist {
		return nil, fmt.Errorf("user not exist")
	}
	var u model.User
	if err := rows.Scan(&u.ID, &u.Name, &u.Email); err != nil {
		return nil, err
	}

	return &u, nil
}

func (r *Repository) GetAllPatterns() ([]model.PatternAnswer, error) {
	rows, err := r.DB.Query(`SELECT p.id, p.user_id, p.title, p.created_at, p.diags, p.is_public, u.name FROM patterns p JOIN users u ON u.id = p.user_id`)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var patterns []model.PatternAnswer
	for rows.Next() {
		var p model.PatternAnswer
		var diagsJSON []byte
		if err := rows.Scan(&p.ID, &p.UserID, &p.Title, &p.CreatedAt, &diagsJSON, &p.IsPublic, &p.AuthorName); err != nil {
			return nil, err
		}

		if err := json.Unmarshal(diagsJSON, &p.Diag); err != nil {
			return nil, err
		}
		patterns = append(patterns, p)
	}
	return patterns, nil
}

func (r *Repository) GetPatternsByUser(userID int) ([]model.PatternAnswer, error) {
	fmt.Println(userID)
	rows, err := r.DB.Query(`SELECT p.id, p.user_id, p.title, p.created_at, p.diags, p.is_public, u.name FROM patterns p JOIN users u ON u.id = p.user_id WHERE p.user_id=$1`, userID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var patterns []model.PatternAnswer
	for rows.Next() {
		fmt.Println("abos")
		var p model.PatternAnswer
		var diagsJSON []byte
		if err := rows.Scan(&p.ID, &p.UserID, &p.Title, &p.CreatedAt, &diagsJSON, &p.IsPublic, &p.AuthorName); err != nil {
			return nil, err
		}

		if err := json.Unmarshal(diagsJSON, &p.Diag); err != nil {
			return nil, err
		}
		patterns = append(patterns, p)
	}
	return patterns, nil
}

func (r *Repository) GetPatternById(patternID int) (*model.PatternAnswer, error) {
	fmt.Println(patternID)
	rows, err := r.DB.Query(`SELECT p.id, p.user_id, p.title, p.created_at, p.diags, p.is_public, u.name FROM patterns p JOIN users u ON u.id = p.user_id WHERE p.id=$1`, patternID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var patterns []model.PatternAnswer
	for rows.Next() {
		fmt.Println("abracadabra")
		var p model.PatternAnswer
		var diagsJSON []byte
		if err := rows.Scan(&p.ID, &p.UserID, &p.Title, &p.CreatedAt, &diagsJSON, &p.IsPublic, &p.AuthorName); err != nil {
			return nil, err
		}

		if err := json.Unmarshal(diagsJSON, &p.Diag); err != nil {
			return nil, err
		}
		patterns = append(patterns, p)
	}
	return &patterns[0], nil
}

func (r *Repository) CreatePattern(p *model.Pattern) error {
	diagsJSON, err := json.Marshal(p.Diag)
	if err != nil {
		return err
	}

	_, err = r.DB.Exec(`
		INSERT INTO patterns (user_id, created_at, title, diags, is_public)
		VALUES ($1,$2,$3,$4,TRUE)`,
		p.UserID, p.CreatedAt, p.Title, diagsJSON,
	)
	return err
}

func (r *Repository) DeletePattern(patternID, authorID int) error {
	fmt.Println("xdxdxd", patternID, authorID)
	_, err := r.DB.Exec(`
		DELETE FROM patterns
		WHERE id = $1 AND user_id = $2
	`, patternID, authorID)
	// if err != nil
	// _, err = r.DB.Exec(`
	// 	DELETE FROM comments
	// 	WHERE pattern_id = $1
	// `, patternID, authorID)
	// fmt.Println(err)

	return err
}

func (r *Repository) GetCommentsForPattern(patternID int) ([]model.Comment, error) {
	rows, err := r.DB.Query(`SELECT c.id, c.user_id, c.pattern_id, c.text, u.name FROM comments c JOIN users u on c.user_id = u.id WHERE c.pattern_id=$1`, patternID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var comments []model.Comment
	for rows.Next() {
		var c model.Comment
		if err := rows.Scan(&c.ID, &c.UserID, &c.PatternID, &c.Text, &c.UserName); err != nil {
			return nil, err
		}
		comments = append(comments, c)
	}
	return comments, nil
}

func (r *Repository) AddComment(c *model.Comment) error {
	if c.UserID == 0 || c.PatternID == 0 {
		return errors.New("invalid comment")
	}
	_, err := r.DB.Exec(`INSERT INTO comments (user_id, pattern_id, text) VALUES ($1,$2,$3)`, c.UserID, c.PatternID, c.Text)
	return err
}

func (r *Repository) UpdateUserLogin(userID int, newLogin string) error {
	if userID == 0 || newLogin == "" {
		return errors.New("invalid user data")
	}

	_, err := r.DB.Exec(`
		UPDATE users
		SET name = $1
		WHERE id = $2
	`, newLogin, userID)

	return err
}
