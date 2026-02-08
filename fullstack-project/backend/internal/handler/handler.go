package handler

import (
	"encoding/json"
	"fmt"
	"fs-api/internal/jwt_auth"
	"fs-api/internal/model"
	"fs-api/internal/service"
	"net/http"
	"strconv"

	"github.com/go-chi/chi"
)

type Handler struct {
	Service *service.Service
}

func (h *Handler) RegisterUser(w http.ResponseWriter, r *http.Request) {
	var user model.User
	if err := json.NewDecoder(r.Body).Decode(&user); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	if user.Email == "" || user.Name == "" || user.Password == "" {
		fmt.Println("some creds are empty", user)
		http.Error(w, "some creds are empty", http.StatusBadRequest)
		return
	}
	if err := h.Service.RegisterUser(&user); err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusCreated)

	var (
		tokenizedUser model.TokenUser
		err           error
	)
	tokenizedUser.Login = user.Name
	tokenizedUser.ID = user.ID
	tokenizedUser.Token, err = jwt_auth.GenerateJWT(user.ID)
	if err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	if err := json.NewEncoder(w).Encode(tokenizedUser); err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
}

func (h *Handler) LoginUser(w http.ResponseWriter, r *http.Request) {
	var user model.User
	if err := json.NewDecoder(r.Body).Decode(&user); err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}
	err := h.Service.LoginUser(&user)
	if err != nil {
		fmt.Println(err)
		http.Error(w, "login error", http.StatusBadRequest)
		return
	}
	w.Header().Set("Content-Type", "application/json")
	var (
		tokenizedUser model.TokenUser
	)
	tokenizedUser.Login = user.Name
	tokenizedUser.ID = user.ID
	tokenizedUser.Token, err = jwt_auth.GenerateJWT(user.ID)
	if err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	if err := json.NewEncoder(w).Encode(tokenizedUser); err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
}

func (h *Handler) GetAllPatterns(w http.ResponseWriter, r *http.Request) {
	patterns, err := h.Service.GetAllPatterns()
	if err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	json.NewEncoder(w).Encode(patterns)
}

func (h *Handler) GetPatternsByUser(w http.ResponseWriter, r *http.Request) {
	userIDStr := chi.URLParam(r, "userID")
	if userIDStr == "" {
		http.Error(w, "userID is required", http.StatusBadRequest)
		return
	}
	userID, err := strconv.Atoi(userIDStr)
	if err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	patterns, err := h.Service.GetPatternsByUser(userID)
	if err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	if patterns == nil {
		patterns = []model.PatternAnswer{}
	}
	json.NewEncoder(w).Encode(patterns)
}

func (h *Handler) CreatePattern(w http.ResponseWriter, r *http.Request) {
	ctx := r.Context()
	var patternReq model.PatternRequest
	if err := json.NewDecoder(r.Body).Decode(&patternReq); err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	var (
		req_id int
		ok     bool
	)
	if req_id, ok = ctx.Value("user_id").(int); !ok {
		http.Error(w, "possibly problems with token1", http.StatusUnauthorized)
		return
	}

	if req_id != patternReq.UserID {
		http.Error(w, "possibly problems with token2", http.StatusUnauthorized)
		return
	}

	if err := h.Service.CreatePattern(&patternReq); err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	w.WriteHeader(http.StatusCreated)
}

func (h *Handler) GetCommentsForPattern(w http.ResponseWriter, r *http.Request) {
	patternID, _ := strconv.Atoi(chi.URLParam(r, "patternID"))
	fmt.Printf("xd")
	comments, _ := h.Service.GetCommentsForPattern(patternID)
	if comments == nil {
		comments = []model.Comment{}
	}
	json.NewEncoder(w).Encode(comments)
}

func (h *Handler) AddComment(w http.ResponseWriter, r *http.Request) {
	var (
		comment model.Comment
		req_id  int
		ok      bool
	)
	ctx := r.Context()

	if req_id, ok = ctx.Value("user_id").(int); !ok {
		http.Error(w, "possibly problems with token1", http.StatusUnauthorized)
		return
	}

	json.NewDecoder(r.Body).Decode(&comment)
	if req_id != comment.UserID {
		http.Error(w, "possibly problems with token2", http.StatusUnauthorized)
		return
	}
	comment.PatternID, _ = strconv.Atoi(chi.URLParam(r, "patternID"))
	h.Service.AddComment(&comment)
	w.WriteHeader(http.StatusCreated)
}

func (h *Handler) DeletePattern(w http.ResponseWriter, r *http.Request) {
	var (
		userID int
		ok     bool
	)
	ctx := r.Context()

	if userID, ok = ctx.Value("user_id").(int); !ok {
		http.Error(w, "possibly problems with token1", http.StatusUnauthorized)
		return
	}
	PatternID, _ := strconv.Atoi(chi.URLParam(r, "patternID"))
	err := h.Service.DeletePattern(PatternID, userID)
	if err != nil {
		http.Error(w, "possibly problems with token2", http.StatusUnauthorized)
		return
	}
	w.WriteHeader(http.StatusAccepted)
}

func (h *Handler) GetUsernameById(w http.ResponseWriter, r *http.Request) {
	userID, _ := strconv.Atoi(chi.URLParam(r, "userID"))
	user, err := h.Service.GetUsernameById(userID)
	if err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	user.Email = ""
	json.NewEncoder(w).Encode(user)
}

func (h *Handler) GetPatternById(w http.ResponseWriter, r *http.Request) {
	patternID, _ := strconv.Atoi(chi.URLParam(r, "patternID"))
	pattern, err := h.Service.GetPatternById(patternID)
	if err != nil {
		fmt.Println(err)
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	json.NewEncoder(w).Encode(pattern)
}

func (h *Handler) UpdateUserLogin(w http.ResponseWriter, r *http.Request) {
	var body struct {
		Login string `json:"login"`
	}

	ctx := r.Context()

	userID, ok := ctx.Value("user_id").(int)
	if !ok {
		http.Error(w, "unauthorized", http.StatusUnauthorized)
		return
	}
	paramID, err := strconv.Atoi(chi.URLParam(r, "userID"))
	if err != nil {
		http.Error(w, "invalid user id", http.StatusBadRequest)
		return
	}
	if userID != paramID {
		http.Error(w, "forbidden", http.StatusForbidden)
		return
	}
	if err := json.NewDecoder(r.Body).Decode(&body); err != nil {
		http.Error(w, "invalid body", http.StatusBadRequest)
		return
	}
	if body.Login == "" {
		http.Error(w, "login cannot be empty", http.StatusBadRequest)
		return
	}
	if err := h.Service.UpdateUserLogin(userID, body.Login); err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusOK)
	json.NewEncoder(w).Encode(map[string]string{
		"login": body.Login,
	})
}
