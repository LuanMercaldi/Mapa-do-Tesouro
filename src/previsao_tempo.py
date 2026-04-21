import requests

# Dados para usar o OpenWeatherMap

API_KEY = '180d6f5eef80c5d38f7219e8be7aa93b'
CIDADE = 'Itápolis, BR'

def verificar_previsao_tempo():
    url = f'http://api.openweathermap.org/data/2.5/weather?q={CIDADE}&appid={API_KEY}&lang=pt_br'
    try:
        resposta = requests.get(url)
        dados = resposta.json()

        # Pega a Descrição do clima
        clima = dados['weather'][0]['description'].lower()

        print(f'🌦️ Clima atual na lavoura de {CIDADE}: {clima}')

        # Verfica se as palavras 'chuva' ou 'chuvoso' estão na descrição
        if 'chuva' in clima or 'chuvoso' in clima:
            print('-> ALERTA: Previsão de CHUVA detectada!')
            print('-> AÇÃO NO WOKWI: Digite a letra "C" no Monitor Serial para SUSPENDER a irrigação.')
        else:
            print('-> STATUS: Sem previsão de chuva')
            print('-> AÇÃO NO WOKWI: Digite a letra "S" no Monitor Serial para MANTER o sistema automático.')

    except Exception as e:
        print('Erro ao consultar a API:', e)

if __name__ == '__main__':
    verificar_previsao_tempo()
