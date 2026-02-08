package main

import (
	"database/sql"
	"fmt"
	"fs-api/internal/handler"
	"fs-api/internal/jwt_auth"
	"fs-api/internal/repository"
	"fs-api/internal/service"
	"fs-api/internal/table"
	"log"
	"net/http"
	"os"

	"github.com/go-chi/chi"

	_ "github.com/GoAdminGroup/go-admin/adapter/chi"
	"github.com/GoAdminGroup/go-admin/engine"
	"github.com/GoAdminGroup/go-admin/modules/config"
	_ "github.com/GoAdminGroup/go-admin/modules/db/drivers/postgres"
	"github.com/GoAdminGroup/go-admin/modules/language"
	_ "github.com/GoAdminGroup/themes/adminlte"
	_ "github.com/jackc/pgx/v5/stdlib"
)

func main() {
	dsn := fmt.Sprintf(
		"host=%s port=%s user=%s password=%s dbname=%s sslmode=%s",
		os.Getenv("DB_HOST"),
		os.Getenv("DB_PORT"),
		os.Getenv("DB_USER"),
		os.Getenv("DB_PASSWORD"),
		os.Getenv("DB_NAME"),
		os.Getenv("DB_SSLMODE"),
	)
	db, err := sql.Open("pgx", dsn)
	fmt.Println(db, err)
	db.SetMaxIdleConns(50)
	db.SetMaxOpenConns(150)
	if err != nil {
		log.Fatal(err)
	}

	eng := engine.Default()

	config := &config.Config{
		Databases: config.DatabaseList{

			"default": {
				Name:         os.Getenv("DB_NAME"),
				Host:         os.Getenv("DB_HOST"),
				Port:         os.Getenv("DB_PORT"),
				User:         os.Getenv("DB_USER"),
				Pwd:          os.Getenv("DB_PASSWORD"),
				MaxIdleConns: 1,
				MaxOpenConns: 1,
				Driver:       config.DriverPostgresql,
			},
		},
		Store: config.Store{
			Path:   "./uploads",
			Prefix: "uploads",
		},
		UrlPrefix: "api/admin",
		Language:  language.EN,
	}
	r := chi.NewRouter()
	r.Use(CORSMiddleware)

	repo := &repository.Repository{DB: db}
	service := &service.Service{Repo: repo}
	handler := &handler.Handler{Service: service}
	r.Route("/api", func(r chi.Router) {
		r.Post("/register", handler.RegisterUser)
		r.Post("/login", handler.LoginUser)

		r.Group(func(r chi.Router) {
			r.Use(jwt_auth.JWTMiddleware)
			r.Post("/patterns/{patternID}/comments", handler.AddComment)
			r.Post("/patterns", handler.CreatePattern)

			r.Delete("/patterns/{patternID}", handler.DeletePattern)
			r.Put("/users/{userID}", handler.UpdateUserLogin)
		})

		r.Get("/patterns", handler.GetAllPatterns)
		r.Get("/patterns/user/{userID}", handler.GetPatternsByUser)

		r.Get("/users/{userID}", handler.GetUsernameById)

		r.Get("/patterns/{patternID}/comments", handler.GetCommentsForPattern)
		r.Get("/patterns/{patternID}", handler.GetPatternById)
	})
	eng.AddConfig(config).AddGenerators(table.Generators).Use(r)
	fmt.Printf("Мы поднялись, капитан\n")
	e := http.ListenAndServe(":1988", r)
	fmt.Println(e)
}
