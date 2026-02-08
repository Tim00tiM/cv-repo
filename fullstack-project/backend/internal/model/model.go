package model

type User struct {
	ID           int    `json:"id"`
	Name         string `json:"login"`
	Email        string `json:"email"`
	Password     string `json:"password"`
	PasswordHash string `json:"-"`
}

type Comment struct {
	ID        int    `json:"id"`
	UserID    int    `json:"author_id"`
	UserName  string `json:"author_name"`
	PatternID int    `json:"pattern_id"`
	Text      string `json:"text"`
}

type Pattern struct {
	ID        int         `json:"id"`
	UserID    int         `json:"author_id"`
	CreatedAt string      `json:"creation_date"`
	Diag      PatternData `json:"diag"`
	IsPublic  bool        `json:"is_public"`
	Title     string      `json:"title"`
}

type PatternData struct {
	Diags  []IDiag `json:"diags"`
	Width  int     `json:"width"`
	Height int     `json:"height"`
}

type IDiag struct {
	Colors []IColor `json:"colors"`
}

type IColor struct {
	Red   int `json:"red"`
	Green int `json:"green"`
	Blue  int `json:"blue"`
}

type TokenUser struct {
	Token string `json:"token"`
	ID    int    `json:"self_id"`
	Login string `json:"login"`
}

type PatternRequest struct {
	Pattern
	Token string `json:"token"`
}

type PatternAnswer struct {
	Pattern
	AuthorName string `json:"author_name"`
}
