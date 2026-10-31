if [ -n "$IS_ENV_LOADED" ]; then
    echo -e "\033[33mEnvironment already loaded, skipping...\033[0m"
    return
fi

if [ "$ENV_ENTRY" = "vscode" ]; then
    source ~/.bashrc
fi

echo -e "\033[32mEnvironment load completed!\033[0m"
export IS_ENV_LOADED=1