package service

import (
	"fs-api/internal/model"
	"fs-api/internal/repository"

	"crypto/sha256"
	"encoding/hex"
)

type Service struct {
	Repo *repository.Repository
}

func (s *Service) RegisterUser(user *model.User) error {
	hash := sha256.Sum256([]byte(user.Password))
	hashString := hex.EncodeToString(hash[:])
	user.PasswordHash = hashString

	return s.Repo.CreateUser(user)
}

func (s *Service) LoginUser(user *model.User) error {
	hash := sha256.Sum256([]byte(user.Password))
	hashString := hex.EncodeToString(hash[:])
	user.PasswordHash = hashString

	return s.Repo.CheckCredentials(user)
}

func (s *Service) GetAllPatterns() ([]model.PatternAnswer, error) {
	return s.Repo.GetAllPatterns()
}

func (s *Service) GetPatternsByUser(userID int) ([]model.PatternAnswer, error) {
	return s.Repo.GetPatternsByUser(userID)
}

func (s *Service) CreatePattern(pattern *model.PatternRequest) error {
	return s.Repo.CreatePattern(&pattern.Pattern)
}

func (s *Service) GetCommentsForPattern(patternID int) ([]model.Comment, error) {
	return s.Repo.GetCommentsForPattern(patternID)
}

func (s *Service) AddComment(comment *model.Comment) error {
	return s.Repo.AddComment(comment)
}

func (s *Service) DeletePattern(patternID, authorID int) error {
	return s.Repo.DeletePattern(patternID, authorID)
}

func (s *Service) GetUsernameById(userID int) (*model.User, error) {
	return s.Repo.GetUserInfo(userID)
}

func (s *Service) GetPatternById(patternID int) (*model.PatternAnswer, error) {
	return s.Repo.GetPatternById(patternID)
}

func (s *Service) UpdateUserLogin(userID int, newLogin string) error {
	return s.Repo.UpdateUserLogin(userID, newLogin)
}
